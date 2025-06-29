// [ginkgo-engine] scene2d.h - defines parameters for a 2D scene.

#pragma once

#include "ginkgopch.h"
#include "ParticleSystem.h" // may use 2d particle system
#include "shader.h"
#include "rendercomponent.h"
#include "Framebuffer.h"
#include "RefWrapper.h"
#include <sol/sol.hpp>

class Scene2D {
public:
	static Scene2D& GetInstance();

	void Init(int width, int height, GLFWwindow* workerSharedContext);
	void Draw(float deltaTime);

	void Shutdown();
	GLuint GetOutputFramebuffer() {
		return outputFramebuffer->GetTextureID();
	}
	static void CreateScene2DLuaBindings(std::shared_ptr<sol::state> lua);
	void SendScene2DinstanceToLua(std::shared_ptr<sol::state> lua);
	void addToRenderComponents(RenderComponent& rc);
	void SetRenderComponentPosition(int id, glm::vec3(_position));
	void SetRenderComponentRotation(int id, glm::vec3(_rotation));

private:
	
	void CompileShaders();
	Shader* shader2d;

	Framebuffer* outputFramebuffer;

	int width, height;
	
	// these are likely to be constant (view may change) but fine to keep here.
	glm::mat4 projection2d;
	glm::mat4 view2d;

	std::vector<Ref<RenderComponent>> renderComponents;

	std::vector<RenderComponent*> demoComponents;
};