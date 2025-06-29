#include "ginkgopch.h"
#include "Audio.h"

Implementation::Implementation() {
	mpStudioSystem = NULL;
	CAudioEngine::ErrorCheck(FMOD::Studio::System::create(&mpStudioSystem));
	CAudioEngine::ErrorCheck(mpStudioSystem->initialize(32, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_PROFILE_ENABLE, NULL));

	mpSystem = NULL;
	CAudioEngine::ErrorCheck(mpStudioSystem->getCoreSystem(&mpSystem));
}

Implementation::~Implementation() {
	CAudioEngine::ErrorCheck(mpStudioSystem->unloadAll());
	CAudioEngine::ErrorCheck(mpStudioSystem->release());
}

void Implementation::Update() {
	vector<ChannelMap::iterator> pStoppedChannels;
	for (auto it = mChannels.begin(), itEnd = mChannels.end(); it != itEnd; ++it) {
		bool bIsPlaying = false;
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying) {
			pStoppedChannels.push_back(it);
		}
	}
	for (auto& it : pStoppedChannels) {
		mChannels.erase(it);
	}
	CAudioEngine::ErrorCheck(mpStudioSystem->update());
}

CAudioEngine& CAudioEngine::getInstance() {
	static CAudioEngine instance;
	return instance;
}

CAudioEngine::CAudioEngine() {
	//sgpImplementation = new Implementation();
	printf("called audio engine constructor\n");
}

void CAudioEngine::Initialize() {
	sgpImplementation = new Implementation();
}
void CAudioEngine::RegisterCallbacks() {
	auto& settings = SettingsManager::GetInstance();
	settings.RegisterCallback("master_volume",
		[this](const SettingValue& value) {
			for (auto& it : sgpImplementation->mChannels) {
				it.second->setVolume(std::get<float>(value));
			}
		});

	settings.RegisterCallback("mute",
		[this](const SettingValue& value) {
			SetMute(std::get<bool>(value));
		});
}
void CAudioEngine::Update() {
	sgpImplementation->Update();
}

void CAudioEngine::LoadSound(const string& SoundName, const string& SoundPath, bool bLooping, bool b3d, bool bStream) {
	auto tFoundIt = sgpImplementation->mSounds.find(SoundName);
	if (tFoundIt != sgpImplementation->mSounds.end()) {
		return;
	}

	FMOD_MODE eMode = FMOD_DEFAULT;
	eMode |= b3d ? FMOD_3D : FMOD_2D;
	eMode |= bLooping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	eMode |= bStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	FMOD::Sound* pSound = nullptr;
	CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->createSound(SoundPath.c_str(), eMode, nullptr, &pSound));
	if (pSound) {
		sgpImplementation->mSounds[SoundName] = pSound;
	}
}

void CAudioEngine::UnLoadSound(const std::string& SoundName) {
	auto tFoundIt = sgpImplementation->mSounds.find(SoundName);
	if (tFoundIt == sgpImplementation->mSounds.end()) {
		return;
	}
	CAudioEngine::ErrorCheck(tFoundIt->second->release());
	sgpImplementation->mSounds.erase(tFoundIt);
}

int CAudioEngine::PlaySound(const string& SoundName, bool blooping, const SoundVec3& vPosition, float fVolumeDB) {
	int nChannelId = sgpImplementation->mnNextChannelId++;
	auto tFoundIt = sgpImplementation->mSounds.find(SoundName);
	if (tFoundIt == sgpImplementation->mSounds.end()) {
		throw std::runtime_error("Sound not loaded");
	}
	FMOD::Channel* pChannel = nullptr;
	CAudioEngine::ErrorCheck(sgpImplementation->mpSystem->playSound(tFoundIt->second, nullptr, true, &pChannel));
	if (pChannel) {
		FMOD_MODE currMode;
		tFoundIt->second->getMode(&currMode);
		if (currMode & FMOD_3D) {
			FMOD_VECTOR position = VectorToFmod(vPosition);
			CAudioEngine::ErrorCheck(pChannel->set3DAttributes(&position, nullptr));
		}
		CAudioEngine::ErrorCheck(pChannel->setVolume(DBtoVolume(fVolumeDB)));
		CAudioEngine::ErrorCheck(pChannel->setPaused(false));
		sgpImplementation->mChannels[nChannelId] = pChannel;
	}
	return nChannelId;
}

void CAudioEngine::SetMute(bool isMute) {
	for (auto& it : sgpImplementation->mChannels) {
		it.second->setMute(isMute);
	}
}

void CAudioEngine::SetChannel3dPosition(int nChannelId, const SoundVec3& vPosition) {
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end()) {
		return;
	}
	FMOD_VECTOR position = VectorToFmod(vPosition);
	CAudioEngine::ErrorCheck(tFoundIt->second->set3DAttributes(&position, NULL));
}

void CAudioEngine::SetChannelVolume(int nChannelId, float fVolumeDB) {
	auto tFoundIt = sgpImplementation->mChannels.find(nChannelId);
	if (tFoundIt == sgpImplementation->mChannels.end()) {
		return;
	}
	CAudioEngine::ErrorCheck(tFoundIt->second->setVolume(DBtoVolume(fVolumeDB)));
}

void CAudioEngine::LoadBank(const std::string& BankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags) {
	auto tFoundIt = sgpImplementation->mBanks.find(BankName);
	if (tFoundIt != sgpImplementation->mBanks.end()) {
		return;
	}
	FMOD::Studio::Bank* pBank;
	CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->loadBankFile(BankName.c_str(), flags, &pBank));
	if (pBank) {
		sgpImplementation->mBanks[BankName] = pBank;
	}
}

void CAudioEngine::LoadEvent(const std::string& EventName) {
	auto tFoundIt = sgpImplementation->mEvents.find(EventName);
	if (tFoundIt != sgpImplementation->mEvents.end()) {
		return;
	}
	FMOD::Studio::EventDescription* pEventDescription = NULL;
	CAudioEngine::ErrorCheck(sgpImplementation->mpStudioSystem->getEvent(EventName.c_str(), &pEventDescription));
	if (pEventDescription) {
		FMOD::Studio::EventInstance* pEventInstance = NULL;
		CAudioEngine::ErrorCheck(pEventDescription->createInstance(&pEventInstance));
		if (pEventInstance) {
			sgpImplementation->mEvents[EventName] = pEventInstance;
		}
	}
}

void CAudioEngine::StopEvent(const string& strEventName, bool bImmediate) {
	auto tFoundIt = sgpImplementation->mEvents.find(strEventName);
	if (tFoundIt == sgpImplementation->mEvents.end())
		return;

	FMOD_STUDIO_STOP_MODE eMode;
	eMode = bImmediate ? FMOD_STUDIO_STOP_IMMEDIATE : FMOD_STUDIO_STOP_ALLOWFADEOUT;
	CAudioEngine::ErrorCheck(tFoundIt->second->stop(eMode));
}

FMOD_VECTOR CAudioEngine::VectorToFmod(const SoundVec3& vPosition) {
	FMOD_VECTOR fVec;
	fVec.x = vPosition.x;
	fVec.y = vPosition.y;
	fVec.z = vPosition.z;
	return fVec;
}

void CAudioEngine::CreateAudioEngineLuaBind(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<SoundVec3>(
		"SoundVec3",
		sol::constructors<SoundVec3(), SoundVec3(float, float, float)>(),
		"x", &SoundVec3::x,
		"y", &SoundVec3::y,
		"z", &SoundVec3::z
	);

	lua->new_usertype<CAudioEngine>("CAudioEngine",
		"PlaySound", &CAudioEngine::PlaySound,
		"VolumetoDB", &CAudioEngine::VolumetoDB);
}

void CAudioEngine::sendAudioEngineToLua(std::shared_ptr<sol::state> lua) {
	(*lua)["audioEngine"] = std::ref(*this);
}

float CAudioEngine::DBtoVolume(float db) {
	return powf(10.0f, 0.05f * db);
}

float CAudioEngine::VolumetoDB(float volume) {
	return 20.0f * log10f(volume);
}

int CAudioEngine::ErrorCheck(FMOD_RESULT result) {
	if (result != FMOD_OK) {
		cout << "FMOD ERROR " << result << endl;
		return 1;
	}
	//cout << "FMOD all good" << endl;
	return 0;
}

void CAudioEngine::Shutdown() {
	delete sgpImplementation;
}