#include "ginkgopch.h"
#include "AssetManager.h"
#include "jsonmanager.h"
#include "Audio.h"

//#define ARRAY_SIZE_IN_ELEMENTS(a) : (sizeof(a)/sizeof(a[0]))
// These may need to change because it has to match with the layout in the shader
#define POSITION_LOCATION 0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION 2

// has to live here.
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

AssetManager& assetManager = AssetManager::GetInstance();

GLuint AssetManager::LoadTexture(const std::string& path) {
	unsigned int textureID;
	glGenTextures(1, &textureID);

	std::string fileName(GINKGO_CONTENT_DIRECTORY + path);

	int width, height, nChannels;
	unsigned char* data = stbi_load(
		fileName.c_str(),
		&width,
		&height,
		&nChannels,
		0
	);

	if (data) {
		GLenum format;
		switch (nChannels) {
		case 1: format = GL_RED; break;
		case 3: format = GL_RGB; break;
		case 4: format = GL_RGBA; break;
		default: format = GL_NONE; break;
		}
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(
			GL_TEXTURE_2D,
			0, format,
			width, height,
			0, format,
			GL_UNSIGNED_BYTE, data
		);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);

	} else {
		std::cerr << "Texture failed to load. Path: " << path << std::endl;
		stbi_image_free(data);
		exit(-1);
	}

	// unused
	return textureID;
}

// note: cubemap must be sliced into 6 images in a directory.
// JPEG storage assumed (alpha values don't make sense in a skybox)
GLuint AssetManager::LoadCubemap(const std::string& path) {
	GLuint cubemapTextureId;
	glGenTextures(1, &cubemapTextureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureId);

	const std::vector<std::string> faces{
		"right.png",
		"left.png",
		"top.png",
		"bottom.png",
		"front.png",
		"back.png"
	};

	int width, height, nChannels;
	for (unsigned int i = 0; i < 6; ++i) {
		std::string filename(GINKGO_CONTENT_DIRECTORY + path + "/" + faces[i]);
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nChannels, 0);
		if (data) {
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		} else {
			std::cerr << "Cubemap texture failed to load. Path: " << filename << std::endl;
			stbi_image_free(data);
			exit(-1);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubemapTextureId;
}

AssetManager::AssetManager() {

}

AssetManager& AssetManager::GetInstance() {
	static AssetManager instance;
	return instance;
}

void AssetManager::clearInternalData() {
	// TODO: clear the textures array here
	output.m_Positions.clear();
	output.m_Normals.clear();
	output.m_TexCoords.clear();
	output.m_Triangles.clear();
	output.m_Tangents.clear();
}

AssetManager::ModelOutput& AssetManager::LoadModel(std::string& Filepath) {
	clearInternalData();
	const aiScene* pScene = importer.ReadFile(Filepath.c_str(), aiProcess_Triangulate | 
	aiProcess_GenSmoothNormals |  aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	if (!pScene) {
		std::cout << "Error parsing " << Filepath.c_str() << " : " << importer.GetErrorString();
	}
	else {
		InitFromScene(pScene);
	}
	return output;
}

bool AssetManager::InitFromScene(const aiScene* pScene) {
	m_MeshEntries.resize(pScene->mNumMeshes);
	// TODO: resize mMaterials with pScene->mNumMaterials
	unsigned int NumVertices = 0;
	unsigned int NumTriangles = 0;
	CountVerticesAndIndices(pScene, NumVertices, NumTriangles);
	ReserveSpace(NumVertices, NumTriangles);
	InitAllMeshEntries(pScene);
	return true;
}

void AssetManager::CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumTriangles) {
	for (unsigned int i = 0; i < m_MeshEntries.size(); ++i) {
		// To decide the offset
		m_MeshEntries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_MeshEntries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_MeshEntries[i].BaseVertex = NumVertices;
		m_MeshEntries[i].BaseIndex = NumTriangles*3;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumTriangles += pScene->mMeshes[i]->mNumFaces;
	}
}

void AssetManager::ReserveSpace(unsigned int NumVertices, unsigned int NumTriangles) {
	// Will be used to accumulate all the vertices from the aiScene structure
	// This is to avoid multiple memory reallocations
	output.m_Positions.reserve(NumVertices);
	output.m_Normals.reserve(NumVertices);
	output.m_TexCoords.reserve(NumVertices);
	output.m_Triangles.reserve(NumTriangles);
	output.m_Tangents.reserve(NumVertices);
}

void AssetManager::InitSingleMeshEntry(const aiMesh* p_aiMesh) {
	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < p_aiMesh->mNumVertices; ++i) {
		const aiVector3D& pPos = p_aiMesh->mVertices[i];
		const aiVector3D& pNormal = p_aiMesh->mNormals[i];
		const aiVector3D& pTexCoord = p_aiMesh->HasTextureCoords(0) ? p_aiMesh->mTextureCoords[0][i] : zero3D;
		const aiVector3D& pTan = p_aiMesh->mTangents[i];
		output.m_Positions.push_back(glm::vec4(pPos.x, pPos.y, pPos.z, 1.0f));
		output.m_Normals.push_back(glm::vec3(pNormal.x, pNormal.y, pNormal.z));
		output.m_TexCoords.push_back(glm::vec2(pTexCoord.x, pTexCoord.y));
		output.m_Tangents.push_back(glm::vec3(pTan.x, pTan.y, pTan.z));
	}
	// Populate the triangles vector
	// NumFaces --- number of triangles 
	for (unsigned int i = 0; i < p_aiMesh->mNumFaces; ++i) {
		const aiFace& Face = p_aiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		output.m_Triangles.push_back(glm::ivec3(Face.mIndices[0], Face.mIndices[1], Face.mIndices[2]));
	}
	std::cout << "Finish initializing a single entry " << std::endl;
}

void AssetManager::InitAllMeshEntries(const aiScene* pScene) {
	// traverse through all the assimp meshes and use them to populate our mesh entries
	for (unsigned int i = 0; i < m_MeshEntries.size(); ++i) {
		const aiMesh* p_aiMesh = pScene->mMeshes[i];
		InitSingleMeshEntry(p_aiMesh);
	}
	std::cout << "Finish initializing all meshes " << std::endl;
}

void AssetManager::LoadAssetFromJSON(const std::string jsonfilename, GLFWwindow* sharedContext) {
	// activate shared context in this thread
	glfwMakeContextCurrent(sharedContext);

	std::scoped_lock lock(loadingMutex);
	JSONmanager& JSON_MANAGER = JSONmanager::GetInstance();

	// loading graphics assets
	auto graphic_assets_paths = JSON_MANAGER.findValue("GraphicAssets");
	if (graphic_assets_paths) {
		const rapidjson::Value& graphic_assets_data = graphic_assets_paths->get();

		// load all the name-path pairs
		const rapidjson::Value& model_paths = graphic_assets_data["Models"];
		assert(model_paths.IsObject() && "models paths not formatted as an object");
		const rapidjson::Value& skybox_texture_paths = graphic_assets_data["SkyBoxTextures"];
		assert(skybox_texture_paths.IsObject() && "skybox textures not formatted as an object");
		const rapidjson::Value& texture_paths = graphic_assets_data["Textures"];
		assert(texture_paths.IsObject() && "textures not formatted as an object");

		// load the assets with their names
		rapidjson::Value::ConstMemberIterator itr;
		for (itr = model_paths.MemberBegin(); itr != model_paths.MemberEnd(); ++itr) {
			const std::string model_name = itr->name.GetString();
			std::string model_path = itr->value.GetString();
			// temporary
			output = LoadModel(model_path);
		}
		for (itr = skybox_texture_paths.MemberBegin(); itr != skybox_texture_paths.MemberEnd(); ++itr) {
			const std::string skybox_name = itr->name.GetString();
			std::string skybox_path = itr->value.GetString();
			cubemapTextureId = LoadCubemap(skybox_path);
		}
		for (itr = texture_paths.MemberBegin(); itr != texture_paths.MemberEnd(); ++itr) {
			const std::string texture_name = itr->name.GetString();
			std::string texture_path = itr->value.GetString();
			GLuint textureID = LoadTexture(texture_path);
			m_textureIDs[texture_name] = textureID;
		}
	}
	else {
		throw std::runtime_error("No Graphic assets object in the file");
	}

	// loading audio assets
	auto audio_assets_paths = JSON_MANAGER.findValue("AudioAssets");
	if (audio_assets_paths) {
		const rapidjson::Value& audio_asset_data = audio_assets_paths->get();
		CAudioEngine& audioEngine = CAudioEngine::getInstance();

		// background music
		const rapidjson::Value& bgm_path = audio_asset_data["Background"];
		audioEngine.LoadSound("Background", bgm_path.GetString(), true);

		// game object sound effects
		const rapidjson::Value& soundEffect_paths = audio_asset_data["ObjectSoundEffect"];
		rapidjson::Value::ConstMemberIterator itr;
		for (itr = soundEffect_paths.MemberBegin(); itr != soundEffect_paths.MemberEnd(); ++itr) {
			const std::string soundEffect_name = itr->name.GetString();
			const std::string soundEffect_path = itr->value.GetString();
			audioEngine.LoadSound(soundEffect_name, soundEffect_path, false);
		}
	}
	else {
		throw std::runtime_error("No audio assets object in the file");
	}
	
	std::cout << "finished loading json assets" << std::endl;
	loadingFinished = true;
	loadingCV.notify_one();
	//std::this_thread::yield();

	// deactivate the context when it's done
	glfwMakeContextCurrent(NULL);
}

GLuint AssetManager::RetrieveTextureID(const std::string& textureName) {
	auto value = m_textureIDs.find(textureName);
	if (value != m_textureIDs.end()) {
		return value->second;
	}
	else {
		throw std::runtime_error("no textures with the name loaded");
	}
}

void AssetManager::createLoadingThread(GLFWwindow* workerSharedContext) {
	loadingThread = std::make_unique<std::thread>(&AssetManager::LoadAssetFromJSON, this, "Start.json", workerSharedContext);
}

bool AssetManager::finished() {
	std::cout << "checking if loading thread is done: " << loadingFinished.load() << std::endl;
	return loadingFinished.load(); 
}

void AssetManager::closeDown() {
	m_MeshEntries.clear();
	m_textureIDs.clear();
}
