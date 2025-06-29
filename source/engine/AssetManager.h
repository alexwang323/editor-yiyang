#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "ginkgopch.h"
#include "material.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <utility>

#define GINKGO_CONTENT_DIRECTORY "../../content/"

class AssetManager {
public:
	struct AssetLoaderMeshEntry {
		// This represents a model loaded by assimp
		AssetLoaderMeshEntry() {
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = -1;
		}
		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
	};
	struct ModelOutput {
		bool ifSuccess;
		std::vector<glm::vec4> m_Positions;
		std::vector<glm::vec3> m_Normals;
		std::vector<glm::vec2> m_TexCoords;
		std::vector<glm::vec3> m_Tangents;
		std::vector<glm::ivec3> m_Triangles;
	};
	// Delete copy constructor and assignment operator
	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;
	static AssetManager& GetInstance();
	//bool InitMaterials(const aiScene* pScene, const std::string& Filename); // extract the texture from aiScene

	unsigned int GetCubemapTextureId() { return cubemapTextureId; }
	GLuint RetrieveTextureID(const std::string& textureName);
	ModelOutput& GetModel() { return output; }

	// Threading
	void createLoadingThread(GLFWwindow* workerSharedContext);
	void joinThread() { loadingThread->join(); }
	bool finished();

	void LoadAssetFromJSON(const std::string jsonfilename, GLFWwindow* sharedContext);

	std::condition_variable loadingCV;
	std::mutex loadingMutex;

	void closeDown();

private:
	AssetManager();
	Assimp::Importer importer;

	// asset loading thread
	std::unique_ptr<std::thread> loadingThread;
	std::thread::id loadingThreadId;
	std::atomic<bool> loadingFinished{ false };

	// data structures for storing assets
	std::vector<AssetLoaderMeshEntry> m_MeshEntries;
	// TODO: add a vector of materials when the material class is up
	std::unordered_map<std::string, GLuint> m_textureIDs;
	// Temporary space for vertex stuff before sending them to VaoFromTris
	ModelOutput output;
	GLuint cubemapTextureId;

	void clearInternalData();
	ModelOutput& LoadModel(std::string& Filepath);
	bool InitFromScene(const aiScene* pScene);
	void CountVerticesAndIndices(const aiScene* pScene, unsigned int& NumVertices, unsigned int& NumIndices);
	void ReserveSpace(unsigned int NumVertices, unsigned int NumTriangles);
	void InitSingleMeshEntry(const aiMesh* p_aiMesh);
	void InitAllMeshEntries(const aiScene* pScene);
	// manual texture stuff
	GLuint LoadTexture(const std::string& path);
	GLuint LoadCubemap(const std::string& path);
};