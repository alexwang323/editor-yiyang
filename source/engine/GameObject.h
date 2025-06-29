#pragma once
#include "ginkgopch.h"
#include <sol/sol.hpp>
#include <optional>
#include "transformcomponent.h"
#include "rendercomponent.h"
#include "RefWrapper.h"
#include "collider.h"

class TransformComponent;
class RenderComponent;

class GameObject {
public:
	GameObject(int id, const std::string& _go_name);
	void AddTransfromComponent(TransformComponent& tr);
	void AddRenderComponent(RenderComponent& _render);
	static void CreateGameObjectLuaBindings(std::shared_ptr<sol::state> lua);
	TransformComponent& GetTransform();
	RenderComponent& GetRender();
	void SetParentName(const std::string& _parent_name);
	std::string& GetParentName();
	const std::string& GetName();
	const int& GetID() { return goID; }

private:
	const int goID;
	// Will make them into generic Ref<T> array
	TransformComponent transform;
	RenderComponent render;
	collider collider; // the physics component
	const std::string go_name;
	std::string parent_name;
};