#include "ginkgopch.h"
#include "LeafScriptNode.h"
#include "GameObject.h"

LeafScriptNode::LeafScriptNode(const std::string& lua_script_path, 
	std::shared_ptr<sol::state> lua, const std::string& name)
	: BehaviorNode(name) {
	try {
		auto result = lua->safe_script_file(lua_script_path);
	}
	catch (const sol::error& err) {
		std::cout << "Error loading " << err.what() << std::endl;
	}
	main_script = (*lua)["main"];
	m_scriptMainLoaded = true;
}

bool LeafScriptNode::ProcessLeafLuaScript() {
	if (!m_scriptMainLoaded) {
		std::cout << node_name << " script not loaded! " << std::endl;
		return false;
	}
	sol::table on_enter_script = main_script[1];
	if (!on_enter_script.valid()) {
		std::cout << "Not loading " << node_name << " on_enter script correctly" << std::endl;
		return false;
	}
	lsn_enter = on_enter_script["on_enter"];
	if (!lsn_enter.valid()) {
		std::cout << "Didn't get the correct lua script on_enter function for " << node_name << std::endl;
		return false;
	}
	sol::table on_update_script = main_script[2];
	if (!on_update_script.valid()) {
		std::cout << "Not loading " << node_name << " on_update script correctly" << std::endl;
		return false;
	}
	lsn_update = on_update_script["on_update"];
	if (!lsn_update.valid()) {
		std::cout << "Didn't get the correct lua script on_update function for " << node_name << std::endl;
		return false;
	}
	return true; 
}

BehaviorNode::NodeStatus LeafScriptNode::on_enter() {
	// call lua on_enter function and return the result
	sol::protected_function_result enter_res = lsn_enter();
	if (!enter_res.valid()) {
		sol::error err = enter_res.get<sol::error>();
		std::cout << "calling on_enter in lua causing error: " << err.what() << std::endl;
		return BehaviorNode::NodeStatus::SUSPENDED;
	}
	else {
		set_status(NodeStatus::RUNNING);
		return NodeStatus::RUNNING;
	}
}

BehaviorNode::NodeStatus LeafScriptNode::on_update(float dt) {
	// call lua on_udpate function and return the result
	sol::protected_function_result update_res = lsn_update();
	if (!update_res.valid()) {
		sol::error err = update_res.get<sol::error>();
		std::cout << "calling on_update in lua causing error: " << err.what() << std::endl;
		return BehaviorNode::NodeStatus::SUSPENDED;
	}
	else {
		NodeStatus result = update_res.get<BehaviorNode::NodeStatus>();
		//std::cout << node_name << " result: " << result << std::endl;
		set_status(result);
		return result;
	}
}

BehaviorNode::NodeStatus LeafScriptNode::on_exit() {
	return lsn_exit(); // don't have this yet
}