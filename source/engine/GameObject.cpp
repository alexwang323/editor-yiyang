#include "ginkgopch.h"
#include "GameObject.h"
#include "GameObjectFactory.h"


GameObject::GameObject(int id, const std::string& _go_name)
	:goID(id), 
	go_name(Ref<const std::string>(_go_name))
{}

void GameObject::AddTransfromComponent(TransformComponent& tr) {
	transform = tr;
}

TransformComponent& GameObject::GetTransform() {
	return transform;
	/*if (transform.) {
		TransformComponent& tr_ref = tr->get();
		return tr_ref;
	}
	else {
		throw std::runtime_error("Asking for Transform component which is not added ");
	}*/
}

void GameObject::AddRenderComponent(RenderComponent& _render) {
	render = _render;
}

RenderComponent& GameObject::GetRender() {
	return render;
}

void GameObject::SetParentName(const std::string& _parent_name) {
	parent_name = _parent_name;
}

std::string& GameObject::GetParentName() {
	return parent_name;
}

const std::string& GameObject::GetName() {
	return go_name;
}

void GameObject::CreateGameObjectLuaBindings(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<GameObject>("GameObject",
		sol::constructors<GameObject(int id, const std::string& _go_name)>(),
		"goID", &GameObject::goID,
		"name", &GameObject::go_name,
		"transform", &GameObject::transform,
		"render", &GameObject::render,
		"SetParent", &GameObject::SetParentName,
		"AddTransform", &GameObject::AddTransfromComponent,
		"AddRender", &GameObject::AddRenderComponent
	);
}