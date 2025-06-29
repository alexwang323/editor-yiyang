#include "ginkgopch.h"
#include "ScriptingSystem.h"
#include "GlmLuaBindings.h"
#include "transformcomponent.h"
#include "rendercomponent.h"
#include "GameObject.h"
#include "GameObjectFactory.h"
#include "BehaviorNode.h"
#include "Audio.h"
#include "scene2d.h"
#include "scene.h"
#include "collider.h"
#include "InputSystem.h"
#include "prim_line.h"
#include "prim_sphere.h"
#include "prim_circle.h"
#include "myphysicsdirector.h"
#include "State.h"
#include "StateStack.h"

ScriptingSystem& ScriptingSystem::GetInstance() {
	static ScriptingSystem* instance = new ScriptingSystem();
	return *instance;
}

std::shared_ptr<sol::state> ScriptingSystem::GetLuaState() {
	return lua;
}

ScriptingSystem::ScriptingSystem() {
	lua = std::make_shared<sol::state>();
	lua->open_libraries(sol::lib::base, sol::lib::package, sol::lib::table);
	RegisterLuaBindings();
	SendReferencesToLua();
	AddScriptComponentToSystem("scripts/firstscene.lua", "");
	AddScriptComponentToSystem("scripts/Leaf_MoveToTarget.lua", "Leaf_MoveToTarget");
	AddScriptComponentToSystem("scripts/Leaf_PlaySound.lua", "Leaf_PlaySound");
	ProcessAllScript();
}

void ScriptingSystem::Init() {
	// scriptComponents[0] is for scene script right now
	scriptComponents[0]->Init();
	scriptComponents[0]->renderState();
	sendBehaviorAgentToLua();
}

void ScriptingSystem::AddScriptComponentToSystem(const std::string& lua_script_path, const std::string& behaviorName) {
	if (behaviorName.size() == 0) {
		std::shared_ptr<ScriptComponent> sc_script = std::make_shared<ScriptComponent>(lua_script_path, lua);
		scriptComponents.push_back(sc_script);
	}
	else {
		std::shared_ptr<LeafScriptNode> bsc = std::make_shared<LeafScriptNode>(lua_script_path, lua, behaviorName);
		behavior_scripts.push_back(bsc);
	}
}

bool ScriptingSystem::ProcessAllScript() {
	// scriptComponents[0] is for scene script right now
	if (scriptComponents[0]->ProcessLuaScript() == false) {
		return false;
	}
	for (int i = 0; i < behavior_scripts.size(); ++i) {
		if (behavior_scripts[i]->ProcessLeafLuaScript() == false) {
			return false;
		}
	}
	return true;
}

void ScriptingSystem::Update(float dt) {
	auto res = scriptComponents[0]->update(dt); // call the update function in lua
	if (!res.valid()) {
		sol::error err = res;
		std::cout << "Error running the update script! " << err.what() << std::endl;
	}
	// TODO: check each script component see if it needs to call its function in lua
	// Like for behavior trees, not all agents will be going through the behavior tree processing each frame
}

void ScriptingSystem::RegisterLuaBindings() {
	GlmBindings::CreateGlmBindings(lua);
	TransformComponent::CreateTransformToLuaBind(lua);
	Material::CreateMaterialToLuaBind(lua);
	RenderComponent::CreateRenderCompLuaBindings(lua);
	GameObject::CreateGameObjectLuaBindings(lua);
	GameObjectFactory::CreateGOFluaBind(lua);
	CAudioEngine::CreateAudioEngineLuaBind(lua);
	BehaviorNode::CreateBehaviorNodeLuaBind(lua);
	InputSystem::CreateInputSystemLuaBindings(lua);
	Scene2D::CreateScene2DLuaBindings(lua);
	Scene::CreateSceneLuaBind(lua);
	collider_primitive::CreateColliderPrimitiveLuaBind(lua);
	collider::CreateColliderBindings(lua);
	prim_line::CreatePrimLineLuaBindings(lua);
	prim_half_line::CreatePrimHalfLineBindings(lua);
	prim_sphere::CreatePrimSphereLuaBindings(lua);
	prim_circle::CreatePrimCircleLuaBindings(lua);
	Myphysicsdirector::CreatePhysicsDirectorLuaBind(lua);
	State::CreateLuaStateBind(lua);
	StateStack::CreateLuaStateStackBind(lua);
}

/*ScriptingSystem::~ScriptingSystem() {
	std::cout << "dtor for scripting system is called!" << std::endl;
}*/

void ScriptingSystem::Shutdown() {
	scriptComponents.clear();
	behavior_scripts.clear();
	std::cout << "shutting down scripting system" << std::endl;
}

void ScriptingSystem::SendReferencesToLua() {
	GameObjectFactory& GOF = GameObjectFactory::GetInstance();
	GOF.sendGOFtoLua(lua);
	CAudioEngine& audioEngine = CAudioEngine::getInstance();
	audioEngine.sendAudioEngineToLua(lua);
	Scene& scene = Scene::GetInstance();
	scene.sendSceneToLua(lua);
	Scene2D& scene2D = Scene2D::GetInstance();
	scene2D.SendScene2DinstanceToLua(lua);
	InputSystem& inputSys = InputSystem::GetInstance();
	inputSys.SendInputSystemToLua(lua);
}

void ScriptingSystem::sendBehaviorAgentToLua() {
	GameObjectFactory& GOF = GameObjectFactory::GetInstance();
	(*lua)["sendbehaviorAgent"] = std::ref(GOF.findGameObject("json object"));
	// for testing
	//BehaviorNode::NodeStatus result = behavior_scripts[0]->on_update(0.1f);
	//std::cout << "Result of running the update script for L_MoveToTarget: " << result << std::endl;
}

std::shared_ptr<LeafScriptNode> ScriptingSystem::get_behavior_scripts(int index) {
	return behavior_scripts[index];
}