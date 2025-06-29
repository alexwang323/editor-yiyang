#include "ginkgopch.h"
#include "rendercomponent.h"
#include "AssetManager.h"
#include "shape.h"
#include "shader.h"

int RenderComponent::nextID = 0;

void RenderComponent::Init(MESH_TYPE type, int n) {
	switch (type) {
	case MESH_TYPE::MESH_TYPE_SPHERE:
		mesh = new Sphere(n);
		meshType = MESH_TYPE::MESH_TYPE_SPHERE;
		break;
	case MESH_TYPE::MESH_TYPE_CUBE:
		mesh = new Box();
		meshType = MESH_TYPE::MESH_TYPE_CUBE;
		break;
	case MESH_TYPE::MESH_TYPE_CONE:
		mesh = new Cone(n);
		meshType = MESH_TYPE::MESH_TYPE_CONE;
		break;
	case MESH_TYPE::MESH_TYPE_CYLINDER:
		// TODO
		mesh = new Cylinder(n, 1.0f);
		meshType = MESH_TYPE::MESH_TYPE_CYLINDER;
		break;
	case MESH_TYPE::MESH_TYPE_PLANE:
		mesh = new Plane(n);
		meshType = MESH_TYPE::MESH_TYPE_PLANE;
		break;
	case MESH_TYPE::MESH_TYPE_CAPSULE:
		mesh = new Capsule(n,1.0f,1.0f);
		meshType = MESH_TYPE::MESH_TYPE_CAPSULE;
		break;
	case MESH_TYPE::MESH_TYPE_DISC:
		mesh = new Disc(n);
		meshType = MESH_TYPE::MESH_TYPE_DISC;
		break;
	case MESH_TYPE::MESH_TYPE_QUAD:
		mesh = new Quad();
		meshType = MESH_TYPE::MESH_TYPE_QUAD;
		break;
	// assumes well-formed.
	case MESH_TYPE::MESH_TYPE_FROM_FILE:
		AssetManager& assetManager = AssetManager::GetInstance();
		AssetManager::ModelOutput& mesh_output = assetManager.GetModel();
		mesh = new AssimpModel(mesh_output);
		meshType = MESH_TYPE::MESH_TYPE_FROM_FILE;
		break;
	}

	if (mesh) {
		mesh->SetVisibility(true);
	}

	enabled = true;
}

void RenderComponent::SetMaterial(Material& mat) {
	material = mat;
}

void RenderComponent::SetTransformMatrix(glm::mat4& matrix) {
	transform = matrix;
}

void RenderComponent::SetTransform(TransformComponent& tr) {
	// for lua use actually
	transformComponent = tr;
}

void RenderComponent::Update() {
	++updateCounter;
	if (transformComponent.GetPosition().x < -10.0f) {
		//std::cout << "here, updateCounter: " << updateCounter << std::endl;
	}
	transform = transformComponent.getMatrix();
}

void RenderComponent::Draw(MESH_DRAW_MODE mode, Shader* shader) {
	if (enabled) {
		int loc = glGetUniformLocation(shader->programId, "u_model");
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(transform));

		glm::mat4 inv = glm::inverse(transform);
		loc = glGetUniformLocation(shader->programId, "u_normaltransform");
		glUniformMatrix4fv(loc, 1, GL_FALSE, &inv[0][0]);

		material.BindMaterial(shader);

		if (mesh->Drawable()) {
			mesh->DrawVAO(mode);
		}

		material.UnbindMaterial();
	}
}

void RenderComponent::CreateRenderCompLuaBindings(std::shared_ptr<sol::state> lua) {
	lua->new_enum("MESH_TYPE",
		"MESH_TYPE_SPHERE", MESH_TYPE::MESH_TYPE_SPHERE,
		"MESH_TYPE_CUBE", MESH_TYPE::MESH_TYPE_CUBE,
		"MESH_TYPE_CONE", MESH_TYPE::MESH_TYPE_CONE,
		"MESH_TYPE_PLANE", MESH_TYPE::MESH_TYPE_PLANE,
		"MESH_TYPE_CYLINDER", MESH_TYPE::MESH_TYPE_CYLINDER,
		"MESH_TYPE_CAPSULE", MESH_TYPE::MESH_TYPE_CAPSULE,
		"MESH_TYPE_QUAD", MESH_TYPE::MESH_TYPE_QUAD,
		"MESH_TYPE_FROM_FILE", MESH_TYPE::MESH_TYPE_FROM_FILE
	);
	lua->new_usertype<RenderComponent>("RenderComponent",
		sol::constructors<RenderComponent()>(),
		"id", &RenderComponent::id,
		"material", &RenderComponent::material,
		"meshType", &RenderComponent::meshType,
		"GetTransform", &RenderComponent::GetTransformComponent,
		"Init", &RenderComponent::Init,
		"SetMaterial", &RenderComponent::SetMaterial,
		"SetTransform", &RenderComponent::SetTransform
	);
}
