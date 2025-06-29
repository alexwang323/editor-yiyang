#include "ginkgopch.h"
#include "ScriptComponent.h"

ScriptComponent::ScriptComponent(const std::string& lua_script_path, std::shared_ptr<sol::state> lua) {
	try {
		auto result = lua->safe_script_file(lua_script_path);
	}
	catch (const sol::error& err) {
		std::cout << "Error loading " << err.what() << std::endl;
	}
	main_script = (*lua)["main"];
	m_scriptMainLoaded = true;
}

bool ScriptComponent::ProcessLuaScript() {
	if (!m_scriptMainLoaded) {
		std::cout << "Scene main script not loaded! " << std::endl;
		return false;
	}

	sol::table update_script = main_script[1]; // first script is how to update
	if (!update_script.valid()) {
		std::cout << "Not loading the update script correctly " << std::endl;
		return false;
	}
	sol::table setup_script = main_script[2]; // second script is the setup
	if (!setup_script.valid()) {
		std::cout << "Not loading the setup script correctly " << std::endl;
		return false;
	}
	sol::table render_script = main_script[3]; // third script is the state render state
	if (!render_script.valid()) {
		std::cout << "Not loading the render script correctly " << std::endl;
		return false;
	}

	sol::function update = update_script["update"];
	sc_update = update;
	if (!sc_update.valid()) {
		std::cout << "Didn't get the correct lua script update function " << std::endl;
		return false;
	}
	sol::function setup = setup_script["setup"];
	sc_setup = setup;
	if (!sc_setup.valid()) {
		std::cout << "Didn't get the correct lua script setup function " << std::endl;
		return false;
	}
	sol::function render = render_script["render"];
	sc_render = render;
	if (!sc_render.valid()) {
		std::cout << "Didn't get the correct lua script render function " << std::endl;
		return false;
	}
	return true;
}

sol::protected_function_result ScriptComponent::Init() {
	return sc_setup();
}

sol::protected_function_result ScriptComponent::update(float dt) {
	return sc_update(dt); // call the lua update function and return the result
}

sol::protected_function_result ScriptComponent::renderState() {
	return sc_render();
}