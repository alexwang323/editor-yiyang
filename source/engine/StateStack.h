#pragma once
#include "State.h"

class StateStack {
public:
	StateStack() = default;
	~StateStack() = default;

	void Push(State& state);
	void Pop();
	void ChangeState(State& state);

	void Update(const float dt);
	void Render();

	State& Top();

	static void CreateLuaStateStackBind(std::shared_ptr<sol::state> lua);
private:
	std::vector<State> m_States{};
	std::unique_ptr<State> m_pStateHolder{ nullptr };
};