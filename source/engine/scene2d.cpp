#include "ginkgopch.h"

#include "scene2d.h"

Scene2D& Scene2D::GetInstance() {
	static Scene2D instance;
	return instance;
}

void Scene2D::Init(int width, int height, GLFWwindow* workerSharedContext) {

	CompileShaders();

	outputFramebuffer = new Framebuffer(width, height);
	float ratio = (float)width / (float)height;
	projection2d = glm::ortho(-ratio, ratio, -1.0f, 1.0f, -1.0f, 1.0f);
	view2d = glm::mat4(1.0f); // this can be more complex later.

	this->width = width;
	this->height = height;

	// Load Assets
	/*
		
	*/

	// Temp Components
	/*Material* tempMat = new Material(
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(0.5f),
		1.0f
	);
	TransformComponent* tempTr = new TransformComponent(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(glm::radians(45.0f), 0.0f, 0.0f),
		glm::vec3(0.2f)
	);
	RenderComponent* tempRC = new RenderComponent();
	tempRC->Init(MESH_TYPE_QUAD);
	tempRC->SetMaterial(*tempMat);
	tempRC->SetTransform(*tempTr);
	renderComponents.push_back(Ref(*tempRC));

	// for demo
	demoComponents.push_back(tempRC);*/
}

void Scene2D::Draw(float deltaTime) {

	int loc, programId;

	// update manual components (demo)
	double time = glfwGetTime();
	/*for (Ref<RenderComponent> rc : renderComponents) {
		rc.get().transformComponent.SetRotation(glm::vec3(0.0f,0.0f, 5.0f * time));
		rc.get().transformComponent.SetPosition(glm::vec3(0.5f * glm::cos(time), 0.5f * glm::sin(time), 0.0f));
	}*/

	for (Ref<RenderComponent> rc : renderComponents) {
		rc.get().Update();
	}

	shader2d->UseShader();
	programId = shader2d->programId;

	outputFramebuffer->Bind();

	glViewport(0, 0, width, height);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set uniforms
	loc = glGetUniformLocation(programId, "u_projection");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(projection2d));
	loc = glGetUniformLocation(programId, "u_view");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(view2d));

	for (Ref<RenderComponent> rc : renderComponents) {
		rc.get().Draw(MESH_DRAW_MODE_NORMAL, shader2d);
	}

	// unbind fbo
	outputFramebuffer->Unbind();
	shader2d->UnuseShader();
}

void Scene2D::CompileShaders() {
	shader2d = new Shader();
	shader2d->AddShader("shaders/shader2d.vert", GL_VERTEX_SHADER);
	shader2d->AddShader("shaders/shader2d.frag", GL_FRAGMENT_SHADER);

	glBindAttribLocation(shader2d->programId, 0, "a_vertex");
	glBindAttribLocation(shader2d->programId, 1, "a_vertextexture");

	shader2d->LinkProgram();
}

void Scene2D::Shutdown() {

}

void Scene2D::CreateScene2DLuaBindings(std::shared_ptr<sol::state> lua) {
	// need to bind Gluint
	lua->new_usertype<GLuint>("GLuint");

	lua->new_usertype<Scene2D>("Scene2D",
		"SetRenderComponentPosition", &Scene2D::SetRenderComponentPosition,
		"SetRenderComponentRotation", &Scene2D::SetRenderComponentRotation,
		"addToRenderComponents", &Scene2D::addToRenderComponents,
		"GetOutputFramebuffer", &Scene2D::GetOutputFramebuffer
	);
}

void Scene2D::SendScene2DinstanceToLua(std::shared_ptr<sol::state> lua) {
	(*lua)["scene2D"] = std::ref(*this);
}

void Scene2D::addToRenderComponents(RenderComponent& rc) {
	renderComponents.push_back(Ref(rc));
}

void Scene2D::SetRenderComponentPosition(int id, glm::vec3(_position)){
	// need to add an assert for if the id exists here
	std::cout << "SetRenderComponentPosition called" << std::endl;
	renderComponents.at(0).get().transformComponent.SetPosition(_position);
}

void Scene2D::SetRenderComponentRotation(int id, glm::vec3(_rotation)) {
	// need to add an assert for if the id exists here
	renderComponents.at(0).get().transformComponent.SetRotation(_rotation);
}