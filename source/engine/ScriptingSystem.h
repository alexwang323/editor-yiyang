#pragma once
#include "ScriptComponent.h"
#include "LeafScriptNode.h"

class GameObject;

class ScriptingSystem {
public:
	static ScriptingSystem& GetInstance();
	std::shared_ptr<sol::state> GetLuaState();
	void Init();
	void AddScriptComponentToSystem(const std::string& lua_script_path, const std::string& behaviorName);
	bool ProcessAllScript();
	void RegisterLuaBindings();
	void SendReferencesToLua();

	// temp
	void sendBehaviorAgentToLua();
	std::shared_ptr<LeafScriptNode> get_behavior_scripts(int index);
	
	void Update(float dt);
	void Shutdown();
private:
	ScriptingSystem();
	~ScriptingSystem() = delete;
	std::vector<std::shared_ptr<ScriptComponent>> scriptComponents; // scene scriptComponents
	std::shared_ptr<sol::state> lua;
	std::vector<std::shared_ptr<LeafScriptNode>> behavior_scripts; // not scene scripts
};

