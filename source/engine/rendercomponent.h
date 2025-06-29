// [ginkgo-engine] rendercomponent.h - captures vao + material

#pragma once

#include "ginkgopch.h"
#include "mesh.h"
#include "material.h"
#include "RefWrapper.h"

#define DEFAULT_SUBDIVISIONS 16

class RenderComponent {

friend class Renderer;
friend class ComponentSerializer;
friend class Editor;

// for demo. to be removed.
friend class Scene;
friend class Scene2D;

public:
	RenderComponent() : 
		id(nextID++),
		mesh(nullptr), 
		material(Material()), 
		meshType(MESH_TYPE::MESH_TYPE_MAX_ENUM), 
		transform(glm::mat4(1.0f))
	{};

	//~RenderComponent() {
		//delete mesh;
	//}

	void Init(MESH_TYPE type, int n=DEFAULT_SUBDIVISIONS);

	void SetMeshFile(const std::string& file) { meshFile = file; }
	void SetMaterial(Material& mat);
	void SetTransform(TransformComponent& tr);

	void SetTransformMatrix(glm::mat4& matrix);

	void Update();
	void Draw(MESH_DRAW_MODE mode, Shader* shader);
	static void CreateRenderCompLuaBindings(std::shared_ptr<sol::state> lua);

	TransformComponent& GetTransformComponent() { return transformComponent; }
	Material& GetMaterial() { return material; }
	int& GetID() { return id; }

private:
	static int nextID;
	int id;
	bool enabled = false;

	// dont want to do an ugly cast block, so this is forced to be a ptr
	Mesh* mesh;
	Material material;
	MESH_TYPE meshType;
	std::string meshFile;

	//std::vector<Ref<TransformComponent>> transformComponents;
	TransformComponent transformComponent;
	glm::mat4 transform;
	int updateCounter = 0;
};

/*
	A note on serialization:

	RenderComponent can be entirely reconstructed from the following:
		-a meshfile string (can be empty)
		-MESH_TYPE
		-Material
			-diffuse
			-specular
			-shininess
			-any relevant texture files
		-transform matrix
		-drawable boolean (perhaps the component is built but not visible at construction time)

	FOR NOW, this is enough to reconstruct any given RenderComponent.

	Construction steps:
		1. Renderer would select which RenderComponent this is being built on (might need to resize)
		2. Mesh is constructed according to MESH_TYPE and the additional parameter (would be stored in json)
			2.1 after creating VAO, set drawable=true
		3. Material params can be set (texture loaded if necessary)
		4. transform passed down from TransformComponent (better this way, prevents bad state)
		5. set enabled=true
*/