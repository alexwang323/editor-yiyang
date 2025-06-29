#pragma once
#ifndef _AUDIO_ENGINE_H_
#define _AUDIO_ENGINE_H_

#include "fmod/fmod.hpp"
#include "fmod/fmod_studio.hpp"
#include <string>
#include <map>
#include <vector>
#include <math.h>
#include <iostream>
#include <sol/sol.hpp>
#include "settingsManager.h"

using namespace std;

#endif

struct SoundVec3 {
	float x;
	float y;
	float z;
};


struct Implementation {
	Implementation();
	~Implementation();

	void Update();

	FMOD::Studio::System* mpStudioSystem;
	FMOD::System* mpSystem;

	int mnNextChannelId;

	typedef map<string, FMOD::Sound*> SoundMap;
	typedef map<int, FMOD::Channel*> ChannelMap;
	typedef map<string, FMOD::Studio::EventInstance*> EventMap;
	typedef map<string, FMOD::Studio::Bank*> BankMap;

	BankMap mBanks;
	EventMap mEvents;
	SoundMap mSounds;
	ChannelMap mChannels;
};


class CAudioEngine {
public:
	static CAudioEngine& getInstance();
	CAudioEngine();
	void Initialize();
	void RegisterCallbacks();
	void Update();
	void Shutdown();
	static int ErrorCheck(FMOD_RESULT result);

	void LoadBank(const string& BankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags);
	void LoadEvent(const string& EventName);
	void LoadSound(const string& SoundName, const string& SoundPath, bool blooping, bool b3d = true, bool bStream = false);
	void UnLoadSound(const string& SoundName);
	//void Set3dListnerAndOrientation(const SoundVec3& vPos = SoundVec3{ 0,0,0 }, float fVolumeDB = 0.0f);
	int PlaySound(const string& SoundName, bool blooping, const SoundVec3& vPos = SoundVec3{ 0,0,0 }, float fVolumeDB = 0.0f);
	void SetMute(bool isMute);
	//void PlayEvent(const string& EventName);
	//void StopChannel(int nChannelId);
	void StopEvent(const string& EventName, bool bImmediate = false);
	//void StopAllChannels();
	void SetChannel3dPosition(int nChannelId, const SoundVec3& vPosition);
	void SetChannelVolume(int nChannelId, float fVolumeDB);
	//bool IsPlaying(int nChannelId) const;
	//bool IsEventPlaying(const string& EventName) const;
	float DBtoVolume(float db);
	float VolumetoDB(float volume);
	FMOD_VECTOR VectorToFmod(const SoundVec3& vPosition);

	static void CreateAudioEngineLuaBind(std::shared_ptr<sol::state> lua);

	void sendAudioEngineToLua(std::shared_ptr<sol::state> lua);

	Implementation* sgpImplementation = nullptr;
};