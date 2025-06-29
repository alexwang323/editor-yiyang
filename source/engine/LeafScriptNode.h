#pragma once
#include <memory>
#include "BehaviorNode.h"

class GameObject;

class LeafScriptNode : public BehaviorNode {
public:
	LeafScriptNode(const std::string& lua_script_path, std::shared_ptr<sol::state> lua, const std::string& name);
	bool ProcessLeafLuaScript();

	BehaviorNode::NodeStatus on_enter();
	BehaviorNode::NodeStatus on_update(float dt);
	BehaviorNode::NodeStatus on_exit();

private:
	sol::protected_function lsn_enter{ sol::lua_nil };
	sol::protected_function lsn_update{ sol::lua_nil };
	sol::protected_function lsn_exit{ sol::lua_nil };
	bool m_scriptMainLoaded = false;
	sol::table main_script;
};