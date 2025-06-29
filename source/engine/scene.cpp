#include "ginkgopch.h"

#include "scene.h"
#include "scene2d.h"
#include "AssetManager.h"
#include "shape.h"
#include "prim_sphere.h"
#include "prim_line.h"
#include "renderingconstants.h"
#include "GameObjectFactory.h"
#include "ScriptingSystem.h"
#include "MenuManager.h"
#include "settingsManager.h"
#include "Audio.h"

// for the shared context for worker thread
#include "window.h"

extern GameObjectFactory& GOF;
extern AssetManager& assetManager;

Scene* SCENE = nullptr;

Scene::Scene() :
	front(0.005f),
	back(5000.0f),
	width(800),
	height(600),
	camera(nullptr),
	inputSystem(nullptr),
	gBuffer(0), gPosition(0), gNormal(0), gColor(0), gSpecular(0),
	gBufferShader(nullptr),
	samples(0),
	stencilShader(nullptr),
	multisampleShader(nullptr),
	singlesampleShader(nullptr),
	meshDrawMode(MESH_DRAW_MODE_MAX_ENUM),
	sceneDrawMode(SCENE_DRAW_MODE_MAX_ENUM),
	sceneDebugDisplay(SCENE_DEBUG_DISPLAY_MAX_ENUM),
	lightingShader(nullptr),
	deferredShader(nullptr),
	skyboxShader(nullptr),
	skyboxVAO(0),
	skyboxVBO(0),
	shadowmapShader(nullptr),
	shadowmapFramebuffer(nullptr),
	debugTextureId(0),
	debugShader(nullptr),
	debugFramebuffer(nullptr)
{ SCENE = this; }

Scene& Scene::GetInstance() {
	static Scene instance;
	return instance;
}

#ifdef _DEBUG
void Scene::Init(Framebuffer& framebuffer, GLFWwindow * workerSharedContext, unsigned int _samples) {
	width = framebuffer.width; height = framebuffer.height;
#else
void Scene::Init(GLFWwindow* window, GLFWwindow * workerSharedContext, unsigned int _samples) {
	glfwGetFramebufferSize(window, &width, &height);
#endif

	samples = _samples;
	
	glEnable(GL_DEPTH_TEST);
	if (samples > 1)
		glEnable(GL_MULTISAMPLE);
	glEnable(GL_STENCIL_TEST);

	camera = new Camera(glm::vec3(0.0f, 5.0f, 20.0f), -90.0f, -5.0f, 45.0f);

	front = 0.005f;
	back = 5000.0f;

	meshDrawMode = MESH_DRAW_MODE_NORMAL;
	sceneDrawMode = SCENE_DRAW_MODE_DEFERRED_MSAA;
	sceneDebugDisplay = SCENE_DEBUG_DISPLAY_G_NRM;

	CompileShaders();
	ConfigureGBuffer();
	ConfigureSkybox();
	shadowmapFramebuffer = new Framebuffer(SHADOWMAP_DIMENSIONS_X, SHADOWMAP_DIMENSIONS_Y);
	debugFramebuffer = new Framebuffer(width, height);

	// Loading all the asset for the scene here
	//assetManager.LoadAssetFromJSON("Start.json", workerSharedContext);
	assetManager.createLoadingThread(workerSharedContext);
	WaitForLoadingFinished();
	//while (!assetManager.finished()) {
		//std::this_thread::yield();
	//}
	assetManager.joinThread();

	SettingsManager::GetInstance().LoadSettingsConfig("settings_config.json");
	SettingsManager::GetInstance().LoadSettings("settings_game.json");

	/////////////////////////////////////////////
	// TEMPORARY SCENE DEFINITIONS
	// 
	// USE THESE CONSTRUCTIONS AS REFERENCE
	// FOR LOADING A SCENE AND DESERIALIZING
	// SCENE DATA
	/////////////////////////////////////////////

	// create scene data

	// add sphere to scene
	/*TransformComponent* sphereTr = new TransformComponent(
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(1.0f)
	);
	Material* sphereMat = new Material(
		glm::vec3(0.5f, 0.5f, 1.0f),
		glm::vec3(1.0f),
		0.5f
	);
	sphereMat->LoadTexture(MATERIAL_BIT::ALBEDO_BIT,	"diffuse");
	sphereMat->LoadTexture(MATERIAL_BIT::NORMAL_BIT,	"normal");
	sphereMat->LoadTexture(MATERIAL_BIT::SPECULAR_BIT,	"roughness");
	//sceneRoot->AddChild(new SceneGraphNode(new Sphere(64), sphereMat), sphereTr);
	//sceneRoot->AddChild(new SceneGraphNode(new Sphere(64), sphereMat), sphereTr);
	for (int x = 0; x < 20; ++x)
	{
		auto tmpptr = new collider(TransformComponent(
			glm::vec3(0.5f * (x % 3), 4.0f + (x * 4.0), 0.0f),
			glm::vec3(glm::radians(0.0f), glm::radians(.0f), 0.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		));
		tmpptr->axes.emplace_back(new prim_sphere(1.0f));
		tmpptr->SetScale(glm::vec3(1.0f));
		//tmpptr->set_global_position(glm::vec3(1.f, 4.0f, 1.0f));
		tmpptr->is_affected_by_gravity = true;
		tmpptr->set_restitution(1.0f);
		tmpptr->set_mass(1.0f);
		tmpptr->update_inertia();
		physicsDir.axes.push_back(tmpptr);
		RenderComponent* sphereRC = new RenderComponent();
		sphereRC->Init(MESH_TYPE_SPHERE, 64);
		sphereRC->SetMaterial(*sphereMat);
		sphereRC->SetTransform(*tmpptr);
		std::cout << sphereRC->GetTransformComponent() << std::endl;
		tmpptr->transformRef = sphereRC->GetTransformComponent();
		renderComponents.push_back(Ref(*sphereRC));
		//auto tmppty = new prim_sphere();
		// add sphere to scene

		//sceneRoot->AddChild(new SceneGraphNode(new Sphere(64), sphereMat), tmpptr);
	}
	/*
	{
		"axes":
		[
			{relevant data,"type":enum here}
			{}
			{}
		]
	
	
	}
	
	
	
	*/
	/*auto tmpptr = new collider(TransformComponent(
		glm::vec3(0, 3.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(.0f), glm::radians(15.0f)),
		glm::vec3(1.0f, 1.0f, 1.0f)
	));
	auto tmpcirc = new prim_circle();
	tmpcirc->set_radius(1.0f);
	tmpptr->axes.emplace_back(tmpcirc);
	tmpptr->SetScale(glm::vec3(1.0f));
	//tmpptr->set_global_position(glm::vec3(1.f, 4.0f, 1.0f));
	tmpptr->is_affected_by_gravity = false;
	tmpptr->set_restitution(0.5f);
	tmpptr->set_mass(1.0f);
	tmpptr->update_inertia();
	physicsDir.axes.push_back(tmpptr);
	//auto tmppty = new prim_sphere();
	// add sphere to scene
	auto tester = new TransformComponent(
		glm::vec3(0, 3.0, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0), glm::radians(15.0f)),
		glm::vec3(1.0f, 0.1f, 1.0f)
	);
	RenderComponent* sphereRC = new RenderComponent();
	sphereRC->Init(MESH_TYPE_SPHERE, 64);
	sphereRC->SetMaterial(*sphereMat);
	sphereRC->SetTransform(*sphereTr);

	renderComponents.push_back(Ref(*sphereRC));

	// building plane matrix
	Material* planeMat = new Material(
		glm::vec3(1.0f, 1.0f, 0.5f),
		glm::vec3(0.3f),
		0.1f
	);
	auto planeTr = new collider(TransformComponent(
		glm::vec3(0.0f),
		glm::vec3(glm::radians(-90.0f), 0.0f, glm::radians(-45.0f)),
		glm::vec3(10.0f)
	));
	planeMat->LoadTexture(MATERIAL_BIT::ALBEDO_BIT, "plane");
	//sceneRoot->AddChild(new SceneGraphNode(new Plane(16), planeMat), planeTr);

	RenderComponent* planeRC = new RenderComponent();
	planeRC->Init(MESH_TYPE_PLANE);
	planeRC->SetMaterial(*planeMat);
	planeRC->SetTransform(*planeTr);
	
	
		auto tmpptyl1 = new prim_line();
		tmpptyl1->set_direction(glm::vec3(1, 0, 0));
		planeTr->axes.push_back(tmpptyl1);
		tmpptyl1 = new prim_line();
		tmpptyl1->set_direction(glm::vec3(0, 1, 0));
		planeTr->transformRef = planeRC->GetTransformComponent();
		planeTr->axes.push_back(tmpptyl1);
		planeTr->is_affected_by_gravity = false;
		planeTr->set_restitution(1.0f);
		planeTr->set_frictions(1.00f, 1.00f);
		//planeTr->friction = (0.5f);
		planeTr->set_mass(10000.0f);
		planeTr->update_inertia();
		physicsDir.axes.push_back(planeTr);
	renderComponents.push_back(Ref(*planeRC));
	// add cube to scene
	Material* cubeMat = new Material(
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(0.5f),
		1.0f
	);

	auto cubeTr = new collider(TransformComponent(
				glm::vec3(10.0, 3.0, 0.0f),
				glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
				glm::vec3(1.0f)
			));
	
			auto tmpptyl = new prim_line();
			tmpptyl->set_direction(glm::vec3(1, 0, 0));
			cubeTr->axes.push_back(tmpptyl);
			tmpptyl = new prim_line();
			tmpptyl->set_direction(glm::vec3(0, 0, 1));
	
			cubeTr->axes.push_back(tmpptyl);
			tmpptyl = new prim_line();
			tmpptyl->set_direction(glm::vec3(0, 1, 0));
	
			cubeTr->axes.push_back(tmpptyl);
			
			cubeTr->set_restitution(0.0f);
			cubeTr->set_mass(1.0f);
			cubeTr->set_frictions(1.00f, 1.00f);
			cubeTr->set_rotatable(true);
			cubeTr->update_inertia();
			cubeTr->is_affected_by_gravity = false;
			//cubeTr->set_principleAxisRotationVelocity(glm::vec3(0,0.1, 0));
			physicsDir.axes.push_back(cubeTr);
	
	cubeMat->LoadTexture(MATERIAL_BIT::ALBEDO_BIT, "cube");
	//sceneRoot->AddChild(new SceneGraphNode(new Box(), cubeMat), cubeTr);

	RenderComponent* cubeRC = new RenderComponent();
	cubeRC->Init(MESH_TYPE_CUBE);
	cubeRC->SetMaterial(*cubeMat);
	cubeRC->SetTransform(*cubeTr);
	cubeTr->transformRef = cubeRC->GetTransformComponent();
	renderComponents.push_back(Ref(*cubeRC));

	cubeTr = new collider(TransformComponent(
		glm::vec3(4.0f, 8.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), glm::radians(0.0f)),
		glm::vec3(1.0f)
	));

	tmpptyl = new prim_line();
	tmpptyl->set_direction(glm::vec3(1, 0, 0));
	cubeTr->axes.push_back(tmpptyl);
	tmpptyl = new prim_line();
	tmpptyl->set_direction(glm::vec3(0, 0, 1));

	cubeTr->axes.push_back(tmpptyl);
	tmpptyl = new prim_line();
	tmpptyl->set_direction(glm::vec3(0, 1, 0));

	cubeTr->axes.push_back(tmpptyl);

	cubeTr->set_restitution(0.0f);
	cubeTr->set_mass(1000.0f);
	cubeTr->set_rotatable(true);
	cubeTr->update_inertia();
	cubeTr->is_affected_by_gravity = true;
	physicsDir.axes.push_back(cubeTr);
	cubeTr->set_velocity(glm::vec3(0,-1,0));
	
	cubeMat->LoadTexture(MATERIAL_BIT::ALBEDO_BIT, "cube");
	//sceneRoot->AddChild(new SceneGraphNode(new Box(), cubeMat), cubeTr);

	cubeRC = new RenderComponent();
	cubeRC->Init(MESH_TYPE_CUBE);
	cubeRC->SetMaterial(*cubeMat);
	cubeRC->SetTransform(*cubeTr);
	cubeTr->transformRef = cubeRC->GetTransformComponent();
	renderComponents.push_back(Ref(*cubeRC));





	// add cone to scene
	Material* coneMat = new Material(
		glm::vec3(1.0f, 0.5f, 0.5f),
		glm::vec3(0.3f),
		0.8f
	);
	TransformComponent* coneTr = new TransformComponent(
		glm::vec3(-4.0f, 0.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(1.5f)
	);
	RenderComponent* coneRC = new RenderComponent();
	coneRC->Init(MESH_TYPE_CONE, 256);
	coneRC->SetMaterial(*coneMat);
	coneRC->SetTransform(*coneTr);
	renderComponents.push_back(Ref(*coneRC));


	auto tmpptrs = new collider(TransformComponent(
		glm::vec3(2.0f, 2.0f, 0.0f),
		glm::vec3(glm::radians(0.0f), glm::radians(0.0f), 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	));
	auto tmpcircl = new prim_sphere(1.0f);
	//tmpcirc->set_radius(1.0f);
	tmpptrs->axes.emplace_back(tmpcircl);
	//tmpptrs->SetScale(glm::vec3(1.0f));
	//tmpptr->set_global_position(glm::vec3(1.f, 4.0f, 1.0f));
	auto tes = new prim_line();
	tes->set_direction(glm::vec3(0, 1, 0));
	tmpptrs->axes.push_back(tes);
	tmpptrs->is_affected_by_gravity = true;
	tmpptrs->set_restitution(0.0f);
	//tmpptrs->friction = (0.5f);
	tmpptrs->set_mass(1000.0f);
	tmpptrs->update_inertia();
	tmpptrs->set_rotatable(false);
	physicsDir.axes.push_back(tmpptrs);
	Myphysicsdirector::setPlayer(tmpptrs);
	//player = tmpptrs;
	RenderComponent* cylinderRC = new RenderComponent();
	cylinderRC->Init(MESH_TYPE_CAPSULE, 64);
	cylinderRC->SetMaterial(*sphereMat);
	cylinderRC->SetTransform(*tmpptrs);
	tmpptrs->transformRef = cylinderRC->GetTransformComponent();

	renderComponents.push_back(Ref(*cylinderRC));

	// add a wheelchair model to scene
	TransformComponent* wheelChairTr = new TransformComponent(
		glm::vec3(2.0f, 1.0f, 0.0f),
		glm::vec3(0.0f),
		glm::vec3(1.0f)
	);
	RenderComponent* wheelchairRC = new RenderComponent();
	AssetManager::ModelOutput& chair_output = assetManager.GetModel();
	//wheelchairRC->SetMeshFile(chairAssetFilePath);
	wheelchairRC->Init(MESH_TYPE_FROM_FILE);
	wheelchairRC->SetTransform(*wheelChairTr);
	renderComponents.push_back(Ref(*wheelchairRC));

	// Loading scene game objects from game object factory
	// have to return the object itself, or the name and parent name got erased(just name strings)
	//GameObject go = GOF.makeGameObjectArchetype();
	GameObject& go_found = GOF.findGameObject("json object");
	std::cout << "GameObject name: " << go_found.GetName() << std::endl;
	std::cout << "GameObject transform: " << go_found.GetTransform().GetPosition().x << std::endl;
	renderComponents.push_back(Ref(go_found.GetRender()));
	//GameObject& demo_cube = GOF.findGameObject("demoCube");
	//renderComponents.push_back(Ref(demo_cube.GetRender()));
	GOF.serializeScene(go_found);


	// RUBRIC DEMO: 100 objects
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			auto cubeTrr = new TransformComponent(
				glm::vec3((float)i * 2.5f, -3.0f, (float)j * 2.5f),
				glm::vec3(0.0f),
				glm::vec3(1.0f)
			);
			cubeRC = new RenderComponent();
			cubeRC->Init(MESH_TYPE_CUBE);
			cubeRC->SetMaterial(*cubeMat);
			cubeRC->SetTransform(*cubeTrr);
			renderComponents.push_back(Ref(*cubeRC));
			cubefield.push_back(cubeRC);
		}
	}

	// scene2d render quad
	TransformComponent* quadTr = new TransformComponent(
		glm::vec3(0.0f, 5.0f, -10.0f),
		glm::vec3(0.0f),
		glm::vec3(5.0f)
	);
	Material* quadMat = new Material(
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f),
		0.1f
	);
	quadMat->LoadExistingTexture(MATERIAL_BIT::ALBEDO_BIT, Scene2D::GetInstance().GetOutputFramebuffer());
	//quadMat->LoadTexture(MATERIAL_BIT::ALBEDO_BIT, "cube");
	RenderComponent* quadRC = new RenderComponent();
	quadRC->Init(MESH_TYPE_QUAD);
	quadRC->SetMaterial(*quadMat);
	quadRC->SetTransform(*quadTr);
	renderComponents.push_back(Ref(*quadRC));

	//oi.demo_init();
	
	oit.force = glm::vec3{ 0,1,0 };
	teoit.force = glm::vec3{ 0,1,0 };
	teoit.pos = glm::vec3{ 0,3.0f,0 };
	teoit.min_age = 1.0f;
	teoit.max_age = 5.0f;
	teoit.min_speed = 30.0f;
	teoit.max_speed = 60.0f;
	teoit.min_theta = -3.14f;
	teoit.max_theta = 3.14f;*/
    //-=oit.internal_init();


	
	physicsDir._new_elements_added();
	//physicsDir.set_physicson(true);
}

#ifdef _DEBUG
void Scene::Draw(Framebuffer& framebuffer, float deltaTime) {
	// check if framebuffer modified
	if (framebuffer.width != width || framebuffer.height != height) {
		width = framebuffer.width; height = framebuffer.height;
		ConfigureGBuffer();
		debugFramebuffer->Resize(width, height);
	}
#else
void Scene::Draw(GLFWwindow * window, float deltaTime) {
	int curr_w, curr_h;
	glfwGetFramebufferSize(window, &curr_w, &curr_h);
	if (curr_w != width || curr_h != height) {
		width = curr_w; height = curr_h;
		ConfigureGBuffer();
		debugFramebuffer->Resize(width, height);
	}
	if (!inputSystem) {
		InputSystem::Initialize(window, InputMode::EDITOR);
		inputSystem = &InputSystem::GetInstance();
		inputSystem->RegisterCallbacks();
	}
	inputSystem->ProcessInput(deltaTime);
#endif

	int loc, programId;
	glm::mat4 projection, view;

	double time = glfwGetTime();
	for (RenderComponent* rc : cubefield) {
		glm::vec3 pos = rc->GetTransformComponent().GetPosition();
		rc->GetTransformComponent().SetPosition(glm::vec3(pos.x, glm::sin((pos.x * 0.5f) + (pos.z * 0.5f) + time) - 3.0f, pos.z));
	}

	for (Ref<RenderComponent> rc : renderComponents) {
		rc.get().Update();
		//std::cout << "renderComponents sphere transformComponent address: " << &(renderComponents[1].get().transformComponent) << std::endl;
	}

	glViewport(0, 0, width, height);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//--------------------------------------------------
	//                  Shadow Pass
	//--------------------------------------------------

	shadowmapShader->UseShader();
	programId = shadowmapShader->programId;

	glViewport(0, 0, SHADOWMAP_DIMENSIONS_X, SHADOWMAP_DIMENSIONS_Y);
	shadowmapFramebuffer->Bind();
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// LearnOpenGL implementation, ortho
	float near_plane = 0.005f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	loc = glGetUniformLocation(programId, "u_lightprojection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightProjection));
	loc = glGetUniformLocation(programId, "u_lightview");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightView));

	glCullFace(GL_FRONT);
	for (Ref<RenderComponent> rc : renderComponents) {
		rc.get().Draw(meshDrawMode, shadowmapShader);
	}
	glCullFace(GL_BACK);

#ifdef _DEBUG
	framebuffer.Bind();
#else
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

	shadowmapShader->UnuseShader();
	glm::mat4 lightSpace = lightProjection * lightView;

	//--------------------------------------------------
	//					G BUFFER PASS
	//--------------------------------------------------

	gBufferShader->UseShader();
	programId = gBufferShader->programId;

	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	glViewport(0, 0, width, height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, front, back);
	view = camera->ViewMatrix();

	loc = glGetUniformLocation(programId, "u_projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
	loc = glGetUniformLocation(programId, "u_view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

	for (Ref<RenderComponent> rc : renderComponents) {
		rc.get().Draw(meshDrawMode, gBufferShader);
	}

#ifdef _DEBUG
	framebuffer.Bind();
#else
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

	gBufferShader->UnuseShader();

	//--------------------------------------------------
	//            NON-DEFERRED LIGHTING PASS
	//--------------------------------------------------

	if (sceneDrawMode == SCENE_DRAW_MODE_NO_DEFERRED) {
		lightingShader->UseShader();
		programId = lightingShader->programId;

		glViewport(0, 0, width, height);
		glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set uniforms
		projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, front, back);
		view = camera->ViewMatrix();

		loc = glGetUniformLocation(programId, "u_projection");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
		loc = glGetUniformLocation(programId, "u_view");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));
		loc = glGetUniformLocation(programId, "u_lightpos");
		glUniform3fv(loc, 1, glm::value_ptr(lightPos));
		loc = glGetUniformLocation(programId, "u_lightval");
		glUniform3fv(loc, 1, glm::value_ptr(lightVal));
		loc = glGetUniformLocation(programId, "u_lightamb");
		glUniform3fv(loc, 1, glm::value_ptr(lightAmb));
		loc = glGetUniformLocation(programId, "u_eyepos");
		glUniform3fv(loc, 1, glm::value_ptr(camera->position));

		loc = glGetUniformLocation(programId, "u_shadowmatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightSpace));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, shadowmapFramebuffer->GetTextureID());
		loc = glGetUniformLocation(programId, "s_shadowmap");
		glUniform1i(loc, 3); // magic number. will create "texture bindings" consts later.

		for (Ref<RenderComponent> rc : renderComponents) {
			rc.get().Draw(meshDrawMode, lightingShader);
		}

		lightingShader->UnuseShader();
		return;
	}

	//--------------------------------------------------
	//                  DEBUG QUAD DRAW
	//--------------------------------------------------

	if (sceneDrawMode == SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_QUAD) {
		BuildQuad();

		glViewport(0, 0, width, height);
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		debugShader->UseShader();
		programId = debugShader->programId;

		glActiveTexture(GL_TEXTURE0);
		switch (sceneDebugDisplay) {
		case SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_SHADOW:
			glBindTexture(GL_TEXTURE_2D, shadowmapFramebuffer->GetTextureID());
			break;
		case SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_G_NRM:
			glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);

#ifdef _DEBUG
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, debugFramebuffer->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			framebuffer.Bind();
#else
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, debugFramebuffer->fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
			glBindTexture(GL_TEXTURE_2D, debugFramebuffer->GetTextureID());
			break;
		}
		loc = glGetUniformLocation(programId, "s_buffer");
		glUniform1i(loc, 0);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		debugShader->UnuseShader();
		return;
	}

	//--------------------------------------------------
	//				 MSAA STENCIL PASS
	//--------------------------------------------------

	if (samples > 1) {
		// only writing to stencils on this pass
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glStencilMask(GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		//glDisable(GL_STENCIL_TEST);

		stencilShader->UseShader();
		programId = stencilShader->programId;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gPosition);
		loc = glGetUniformLocation(programId, "g_position_ms");
		glUniform1i(loc, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gNormal);
		loc = glGetUniformLocation(programId, "g_normal_ms");
		glUniform1i(loc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gColor);
		loc = glGetUniformLocation(programId, "g_albedo_ms");
		glUniform1i(loc, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gSpecular);
		loc = glGetUniformLocation(programId, "g_specular_ms");
		glUniform1i(loc, 3);

		loc = glGetUniformLocation(programId, "u_samples");
		glUniform1i(loc, samples);

		BuildQuad();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		stencilShader->UnuseShader();

		// reset masks on testing passes
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		//glStencilMask(GL_FALSE);
		//glEnable(GL_STENCIL_TEST);
		//return;

	//--------------------------------------------------
	//			   MSAA SINGLE SAMPLE PASS
	//--------------------------------------------------
		glStencilFunc(GL_NOTEQUAL, 1, GL_TRUE);

		singlesampleShader->UseShader();
		programId = singlesampleShader->programId;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gPosition);
		loc = glGetUniformLocation(programId, "g_position_ms");
		glUniform1i(loc, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gNormal);
		loc = glGetUniformLocation(programId, "g_normal_ms");
		glUniform1i(loc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gColor);
		loc = glGetUniformLocation(programId, "g_albedo_ms");
		glUniform1i(loc, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gSpecular);
		loc = glGetUniformLocation(programId, "g_specular_ms");
		glUniform1i(loc, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowmapFramebuffer->GetTextureID());
		loc = glGetUniformLocation(programId, "s_shadowmap");
		glUniform1i(loc, 4);

		loc = glGetUniformLocation(programId, "u_eyepos");
		glUniform3fv(loc, 1, glm::value_ptr(camera->position));
		loc = glGetUniformLocation(programId, "u_lightpos");
		glUniform3fv(loc, 1, glm::value_ptr(lightPos));
		loc = glGetUniformLocation(programId, "u_lightval");
		glUniform3fv(loc, 1, glm::value_ptr(lightVal));
		loc = glGetUniformLocation(programId, "u_lightamb");
		glUniform3fv(loc, 1, glm::value_ptr(lightAmb));
		loc = glGetUniformLocation(programId, "u_shadowmatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightSpace));

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		singlesampleShader->UnuseShader();

		// debug early break
		//glStencilFunc(GL_ALWAYS, 1, GL_TRUE);
		//return;

	//--------------------------------------------------
	//			   MSAA MULTI SAMPLE PASS
	//--------------------------------------------------
		glStencilFunc(GL_EQUAL, 1, GL_TRUE);

		multisampleShader->UseShader();
		programId = multisampleShader->programId;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gPosition);
		loc = glGetUniformLocation(programId, "g_position_ms");
		glUniform1i(loc, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gNormal);
		loc = glGetUniformLocation(programId, "g_normal_ms");
		glUniform1i(loc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gColor);
		loc = glGetUniformLocation(programId, "g_albedo_ms");
		glUniform1i(loc, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gSpecular);
		loc = glGetUniformLocation(programId, "g_specular_ms");
		glUniform1i(loc, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowmapFramebuffer->GetTextureID());
		loc = glGetUniformLocation(programId, "s_shadowmap");
		glUniform1i(loc, 4);

		loc = glGetUniformLocation(programId, "u_eyepos");
		glUniform3fv(loc, 1, glm::value_ptr(camera->position));
		loc = glGetUniformLocation(programId, "u_lightpos");
		glUniform3fv(loc, 1, glm::value_ptr(lightPos));
		loc = glGetUniformLocation(programId, "u_lightval");
		glUniform3fv(loc, 1, glm::value_ptr(lightVal));
		loc = glGetUniformLocation(programId, "u_lightamb");
		glUniform3fv(loc, 1, glm::value_ptr(lightAmb));
		loc = glGetUniformLocation(programId, "u_shadowmatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightSpace));

		loc = glGetUniformLocation(programId, "u_samples");
		glUniform1i(loc, samples);

		loc = glGetUniformLocation(programId, "u_debug");
		glUniform1i(loc, sceneDrawMode == SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_STENCIL);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		multisampleShader->UnuseShader();

		// reset stencil func
		glStencilFunc(GL_ALWAYS, 1, GL_TRUE);

	} else {
	//--------------------------------------------------
	//				  DEFERRED LIGHTING
	//--------------------------------------------------

		deferredShader->UseShader();
		programId = deferredShader->programId;

		glViewport(0, 0, width, height);
		glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		loc = glGetUniformLocation(programId, "g_position");
		glUniform1i(loc, 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		loc = glGetUniformLocation(programId, "g_normal");
		glUniform1i(loc, 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gColor);
		loc = glGetUniformLocation(programId, "g_albedo");
		glUniform1i(loc, 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gSpecular);
		loc = glGetUniformLocation(programId, "g_specular");
		glUniform1i(loc, 3);

		loc = glGetUniformLocation(programId, "u_eyepos");
		glUniform3fv(loc, 1, glm::value_ptr(camera->position));
		loc = glGetUniformLocation(programId, "u_lightpos");
		glUniform3fv(loc, 1, glm::value_ptr(lightPos));
		loc = glGetUniformLocation(programId, "u_lightval");
		glUniform3fv(loc, 1, glm::value_ptr(lightVal));
		loc = glGetUniformLocation(programId, "u_lightamb");
		glUniform3fv(loc, 1, glm::value_ptr(lightAmb));
		loc = glGetUniformLocation(programId, "u_shadowmatrix");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(lightSpace));

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, shadowmapFramebuffer->GetTextureID());
		loc = glGetUniformLocation(programId, "s_shadowmap");
		glUniform1i(loc, 4);

		BuildQuad();
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);

		deferredShader->UnuseShader();

		//return;
	}

	//--------------------------------------------------
	//					SKYBOX PASS
	//--------------------------------------------------
	glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glDepthFunc(GL_LEQUAL);
	skyboxShader->UseShader();
	programId = skyboxShader->programId;

	glViewport(0, 0, width, height);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);

#ifdef _DEBUG
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.fbo);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	framebuffer.Bind();
#else
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

	loc = glGetUniformLocation(programId, "u_projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection));
	loc = glGetUniformLocation(programId, "u_view");
	view = glm::mat4(glm::mat3(view));
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, assetManager.GetCubemapTextureId());
	loc = glGetUniformLocation(programId, "s_cubemap");
	glUniform1i(loc, 0);
	// turn off cubemap for deferred shading demo
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
	skyboxShader->UnuseShader();

	projection = glm::perspective(glm::radians(camera->zoom), (float)width / (float)height, front, back);
	view = camera->ViewMatrix();

	oit.pass_info(glm::value_ptr(projection), glm::value_ptr(view));
	oit.render(deltaTime);
	//oi.render(deltaTime);
	teoit.pass_info(glm::value_ptr(projection), glm::value_ptr(view));
	teoit.render(deltaTime);
}

void Scene::CompileShaders() {
	// compile gbuffer shader
	gBufferShader = new Shader();
	gBufferShader->AddShader("shaders/gbuffer.vert", GL_VERTEX_SHADER);
	gBufferShader->AddShader("shaders/gbuffer.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(gBufferShader->programId, 0, "a_vertex");
	glBindAttribLocation(gBufferShader->programId, 1, "a_vertexnormal");
	glBindAttribLocation(gBufferShader->programId, 2, "a_vertextexture");
	glBindAttribLocation(gBufferShader->programId, 3, "a_vertextangent");

	gBufferShader->LinkProgram();

	// compile stencil masking shader
	stencilShader = new Shader();
	stencilShader->AddShader("shaders/stencil.vert", GL_VERTEX_SHADER);
	stencilShader->AddShader("shaders/stencil.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(stencilShader->programId, 0, "a_position");
	glBindAttribLocation(stencilShader->programId, 1, "a_texcoord");

	stencilShader->LinkProgram();

	// compile singlesample shader
	singlesampleShader = new Shader();
	singlesampleShader->AddShader("shaders/singlesample.vert", GL_VERTEX_SHADER);
	singlesampleShader->AddShader("shaders/singlesample.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(singlesampleShader->programId, 0, "a_position");
	glBindAttribLocation(singlesampleShader->programId, 1, "a_texcoord");

	singlesampleShader->LinkProgram();

	// compile multisample shader
	multisampleShader = new Shader();
	multisampleShader->AddShader("shaders/multisample.vert", GL_VERTEX_SHADER);
	multisampleShader->AddShader("shaders/multisample.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(multisampleShader->programId, 0, "a_position");
	glBindAttribLocation(multisampleShader->programId, 1, "a_texcoord");

	multisampleShader->LinkProgram();

	// compile deferred shader
	deferredShader = new Shader();
	deferredShader->AddShader("shaders/deferredbrdf.vert", GL_VERTEX_SHADER);
	deferredShader->AddShader("shaders/deferredbrdf.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(deferredShader->programId, 0, "a_position");
	glBindAttribLocation(deferredShader->programId, 1, "a_texcoord");

	deferredShader->LinkProgram();

	// compile lighting shader
	lightingShader = new Shader();
	lightingShader->AddShader("shaders/brdf.vert", GL_VERTEX_SHADER);
	lightingShader->AddShader("shaders/brdf.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(lightingShader->programId, 0, "a_vertex");
	glBindAttribLocation(lightingShader->programId, 1, "a_vertexnormal");
	glBindAttribLocation(lightingShader->programId, 2, "a_vertextexture");
	glBindAttribLocation(lightingShader->programId, 3, "a_vertextangent");

	lightingShader->LinkProgram();

	// compile skybox shader
	skyboxShader = new Shader();
	skyboxShader->AddShader("shaders/cubemap.vert", GL_VERTEX_SHADER);
	skyboxShader->AddShader("shaders/cubemap.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(skyboxShader->programId, 0, "a_vertex");

	skyboxShader->LinkProgram();

	// compile shadowmap shader
	shadowmapShader = new Shader();
	shadowmapShader->AddShader("shaders/shadow.vert", GL_VERTEX_SHADER);
	shadowmapShader->AddShader("shaders/shadow.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(shadowmapShader->programId, 0, "a_vertex");

	shadowmapShader->LinkProgram();

	// compile shadowmap debug shader
	debugShader = new Shader();
	debugShader->AddShader("shaders/debug.vert", GL_VERTEX_SHADER);
	debugShader->AddShader("shaders/debug.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(debugShader->programId, 0, "a_vertex");
	glBindAttribLocation(debugShader->programId, 1, "a_vertextexture");

	debugShader->LinkProgram();
}

void Scene::ConfigureSkybox() {
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(RenderingConstants::skyboxVertices), &RenderingConstants::skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}

void Scene::ConfigureGBuffer() {
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// i_worldpos
	glGenTextures(1, &gPosition);
	if (samples == 1) {
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gPosition);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, gPosition, 0);
	}

	// i_normalvector
	glGenTextures(1, &gNormal);
	if (samples == 1) {
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	} else {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gNormal);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA16F, width, height, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, gNormal, 0);
	}

	// Kd
	glGenTextures(1, &gColor);
	if (samples == 1) {
		glBindTexture(GL_TEXTURE_2D, gColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColor, 0);
	} else {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gColor);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, gColor, 0);
	}

	// Ks + alpha
	glGenTextures(1, &gSpecular);
	if (samples == 1) {
		glBindTexture(GL_TEXTURE_2D, gSpecular);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gSpecular, 0);
	} else {
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gSpecular);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D_MULTISAMPLE, gSpecular, 0);
	}

	unsigned int attachments[4] = { 
		GL_COLOR_ATTACHMENT0, 
		GL_COLOR_ATTACHMENT1, 
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	if (samples == 1) {
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	} else {
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height);
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::BuildQuad()
{
	if (quadVAO == 0) {
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(RenderingConstants::quadVertices), &RenderingConstants::quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
}

void Scene::Iterate(float delta)
{
	physicsDir.set_physicson(MenuManager::GetInstance().GetCurrentMenuState() == MenuState::NONE);
	physicsDir.update_physics();
	//scripting->Update(delta); // should be here eventually
}

void Scene::priv_init(unsigned index)
{
}

void Scene::WaitForLoadingFinished() {
	std::unique_lock lock(assetManager.loadingMutex);
	assetManager.loadingCV.wait(lock, [this]() { return assetManager.finished(); });
}

void Scene::CreateSceneLuaBind(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<Scene>("Scene",
		"physicsDir", &Scene::physicsDir,
		"addToRenderComponents", &Scene::addToRenderComponentsCollection
	);
}

void Scene::sendSceneToLua(std::shared_ptr<sol::state> lua) {
	(*lua)["scene"] = std::ref(*this);
}

void Scene::Shutdown()
{
	delete camera;

	glDeleteFramebuffers(1, &gBuffer);
	gBufferShader->DeleteShader();

	stencilShader->DeleteShader();
	multisampleShader->DeleteShader();
	singlesampleShader->DeleteShader();

	lightingShader->DeleteShader();
	deferredShader->DeleteShader();
	skyboxShader->DeleteShader();

	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);

	shadowmapShader->DeleteShader();
	delete shadowmapFramebuffer;

	glDeleteTextures(1, &debugTextureId);
	debugShader->DeleteShader();
	
	if (quadVAO) {
		glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
	}
}

void Scene::addToRenderComponentsCollection(RenderComponent& rc) {
	renderComponents.push_back(Ref(rc));
}

void Scene::RegisterGameObject(GameObject* go)
{
	if (!go) {
		return;
	}
	int gid = go->GetID();
	goIDToObj[gid] = go;
}

GameObject* Scene::GetGameObjectByID(int goID)
{
	auto it = goIDToObj.find(goID);
	if (it != goIDToObj.end()) {
		return it->second;
	}
	return nullptr;
}

void Scene::RegisterRenderComponent(int rcID, int goID)
{
	rcToGoID[rcID] = goID;
}

GameObject* Scene::FindGameObjectOfRenderComponent(int rcID)
{
	auto it = rcToGoID.find(rcID);
	if (it != rcToGoID.end()) {
		int gid = it->second;
		return GetGameObjectByID(gid);
	}
	return nullptr;
}