// [ginkgo-engine] scene.h : scene definitions

#pragma once

#include "ginkgopch.h"
#include "ParticleSystem.h"
#include "camera.h"
#include "mesh.h"
#include "myphysicsdirector.h"
#include "shader.h"
#include "material.h"
#include "rendercomponent.h"
#include "Framebuffer.h"
#include "RefWrapper.h"
#include "inputSystem.h"

class GameObject;


#define CUBEMAP_FACE_COUNT 6

// oh great, magic numbers... remove when there's somewhere for them to live.
#define SHADOWMAP_DIMENSIONS_X 1024
#define SHADOWMAP_DIMENSIONS_Y 1024

enum SCENE_DRAW_MODE {
    SCENE_DRAW_MODE_DEFERRED_MSAA = 0,
    SCENE_DRAW_MODE_NO_DEFERRED = 1,
    SCENE_DRAW_MODE_DEBUG_QUAD = 2,
    SCENE_DRAW_MODE_DEBUG_STENCIL = 3,
    SCENE_DRAW_MODE_MAX_ENUM = 0x7FFFFFFF
};

enum SCENE_DEBUG_DISPLAY {
    SCENE_DEBUG_DISPLAY_SHADOW = 0,
    SCENE_DEBUG_DISPLAY_G_NRM = 1,
    SCENE_DEBUG_DISPLAY_MAX_ENUM = 0x7FFFFFFF
};

class Scene {

friend class Editor;
friend class AssetManager;

public:
    static Scene& GetInstance();
#ifdef _DEBUG
    void Init(Framebuffer& framebuffer, GLFWwindow* workerSharedContext, unsigned int _samples);
    void Draw(Framebuffer& framebuffer, float deltaTime);
#else
    void Init(GLFWwindow* window, GLFWwindow* workerSharedContext, unsigned int _samples);
    void Draw(GLFWwindow* window, float deltaTime);
#endif
    void Iterate(float delta = 1.0f);
    //SceneGraphNode* GetSceneRoot() const { return sceneRoot; }
    Camera* GetCamera() { return camera; }

    Myphysicsdirector GetPhysicsDirector() { return physicsDir; }
	Ref<RenderComponent> GetRenderComponents(int index) { return renderComponents[index]; }
    size_t GetRenderComponentsSize() { return renderComponents.size(); }

    static void CreateSceneLuaBind(std::shared_ptr<sol::state> lua);
     void sendSceneToLua(std::shared_ptr<sol::state> lua);

	void setMeshDrawMode(MESH_DRAW_MODE mode) { meshDrawMode = mode; }
	MESH_DRAW_MODE getMeshDrawMode() const { return meshDrawMode; }
	void setSceneDrawMode(SCENE_DRAW_MODE mode) { sceneDrawMode = mode; }
	SCENE_DRAW_MODE getSceneDrawMode() const { return sceneDrawMode; }
	void setSceneDebugDisplay(SCENE_DEBUG_DISPLAY mode) { sceneDebugDisplay = mode; }
	SCENE_DEBUG_DISPLAY getSceneDebugDisplay() const { return sceneDebugDisplay; }

    void Shutdown();

    void RegisterGameObject(GameObject* go);
    void RegisterRenderComponent(int rcID, int goID);
    GameObject* GetGameObjectByID(int goID);
    GameObject* FindGameObjectOfRenderComponent(int rcID);

    InputSystem* GetInputSystem() { return inputSystem; }

private:
    Scene();
    void CompileShaders();
    void ConfigureSkybox();
    void ConfigureGBuffer();

    void WaitForLoadingFinished();

    void BuildQuad();

    float front, back;

    // hard coded light data
    glm::vec3 lightPos = glm::vec3(7.0f, 5.0f, 7.0f);

    glm::vec3 lightVal = glm::vec3(1.0f);
    glm::vec3 lightAmb = glm::vec3(0.1f);

    // framebuffer data
    int width, height;

    Camera* camera;
	InputSystem* inputSystem;

    // note: specular buffer contains specular (.xyz) and rougness (.w) material definitions
    unsigned int gBuffer, gPosition, gNormal, gColor, gSpecular;
    Shader* gBufferShader;

    unsigned int samples;
    Shader* stencilShader;
    Shader* multisampleShader;
    Shader* singlesampleShader;

    MESH_DRAW_MODE meshDrawMode;
    SCENE_DRAW_MODE sceneDrawMode;
    SCENE_DEBUG_DISPLAY sceneDebugDisplay;

    Shader* lightingShader;
    Shader* deferredShader;

    Shader* skyboxShader;
   
    GLuint skyboxVAO;
    GLuint skyboxVBO;

    Shader* shadowmapShader;
    Framebuffer* shadowmapFramebuffer;

    GLuint debugTextureId;
    Shader* debugShader;
    Framebuffer* debugFramebuffer;

    GLuint quadVAO = 0;
    GLuint quadVBO;

    //SceneGraphNode* sceneRoot;
    ParticleSystemO oi;
    
    ParticleSystem1 oit;
    ParticleSystem3DVectorFields teoit;
    Myphysicsdirector physicsDir;
    void priv_init(unsigned index = 0);

    //RenderComponent* demoComponent;
    std::vector<Ref<RenderComponent>> renderComponents;
    void addToRenderComponentsCollection(RenderComponent& rc);

    std::vector<RenderComponent*> cubefield;

    // can't use smart ptr or reference, will result in deref invalid ref in the sol library during shutdown
    // think I shouldn't call dtor for scripting system because of sol
    // gonna take a while to figure this out

    std::unordered_map<int, int> rcToGoID;
    std::unordered_map<int, GameObject*> goIDToObj;
};