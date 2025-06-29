#pragma once
#include "ginkgopch.h"
#include <sol/sol.hpp>

class ScriptComponent {
public:
	ScriptComponent(const std::string& lua_script_path, std::shared_ptr<sol::state> lua);
	bool ProcessLuaScript();
	sol::protected_function_result Init();
	sol::protected_function_result update(float dt);
	sol::protected_function_result renderState();
protected:
	bool m_scriptMainLoaded = false;
	sol::table main_script;
private:
	// later these might be in a vector
	sol::protected_function sc_setup{ sol::lua_nil };
	sol::protected_function sc_update{ sol::lua_nil }; 
	sol::protected_function sc_render{ sol::lua_nil };
};