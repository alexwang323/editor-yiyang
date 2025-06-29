#include "ginkgopch.h"
#include "StateStack.h"

// adapted from https://www.youtube.com/watch?v=H3P7iVt7HAk&list=PL3HUvSWOJR7XRDwVVQqqWO-zyyscb8L-v&index=54

void StateStack::Push(State& state) {
	auto hasState = std::find_if(m_States.begin(), m_States.end(),
		[&](const auto& s) {
			return s.name == state.name; }
	);

	if (hasState == m_States.end()) {
		state.bAddState = true;
		m_pStateHolder = std::make_unique<State>(state);
		return;
	}
	assert(false && "Trying to add a state that is already in the stack! ");
}

void StateStack::Pop() {
	if (m_States.empty()) {
		std::cerr << "Trying to pop from an empty state stack!" << std::endl;
		return;
	}

	auto& top = m_States.back();
	top.bKillState = true; // not actually killing the state here, just setting the flag
}

void StateStack::ChangeState(State& state) {
	if (!m_States.empty())   Pop();
	Push(state);
}

void StateStack::Update(const float dt) {

	if (m_pStateHolder && m_pStateHolder->bAddState) {

		m_States.push_back(*m_pStateHolder);
		
		auto result = m_pStateHolder->on_enter();
		if (result.valid()) {
			try {
				if (!result.valid()) {
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error) {
				std::cerr << error.what() << std::endl;
			}

			m_pStateHolder = nullptr;
		}
	}

	if (m_States.empty())    return;

	auto& topState = m_States.back();
	auto update_result = topState.on_update(dt);

	if (update_result.valid()) {
		try {
			if (!update_result.valid()) {
				sol::error error = update_result;
				throw error;
			}
		}
		catch (const sol::error& error) {
			std::cerr << "Failed to update top state: " << error.what() << std::endl;
			return;
		}
		catch (const std::exception& ex) {
			std::cerr << "Failed to update top state: " << ex.what() << std::endl;
			return;
		}
		catch (...) {
			std::cerr << "Failed to update top state: Error Unknown" << std::endl;
			return;
		}
	}
	

	if (topState.handle_inputs().valid()) {
		try {
			auto result = topState.handle_inputs();
			if (!result.valid()) {
				sol::error error = result;
				throw error;
			}
		}
		catch (const sol::error& error) {
			std::cerr << "Failed to handle top state's inputs " << error.what() << std::endl;
			return;
		}
	}

	if (topState.bKillState) {
		if (topState.on_exit().valid()) {
			try {
				auto result = topState.on_exit();
				if (!result.valid()) {
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error) {
				std::cerr << "Failed to exit top state " << std::endl;
				return;
			}
		}
		m_States.pop_back(); // remove the last element
	}

}

void StateStack::Render() {
	for (const auto& state : m_States) {
		if (state.on_render.valid()) {
			try {
				auto result = state.on_render();
				if (!result.valid()) {
					sol::error error = result;
					throw error;
				}
			}
			catch (const sol::error& error) {
				std::cerr << "Failed to render state " << state.name << "  " << error.what() << std::endl;

			}
		}
	}
}

State& StateStack::Top() {
	assert(!m_States.empty() && "Cannot get the top of an empty stack");
	if (m_States.empty())
		throw std::runtime_error("State stack is empty!");

	return m_States.back();
}

void StateStack::CreateLuaStateStackBind(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<StateStack>(
		"StateStack",
		sol::call_constructor,
		sol::constructors<StateStack()>(),
		"change_state", &StateStack::ChangeState,
		"push", &StateStack::Push,
		"pop", &StateStack::Pop,
		"update", &StateStack::Update,
		"render", &StateStack::Render,
		"top", &StateStack::Top
	);
}