#include "ginkgopch.h"
#include "GlmLuaBindings.h"

void GlmBindings::CreateGlmBindings(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<glm::vec3>("vec3",
		sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
		"x", &glm::vec3::x,
		"y", &glm::vec3::y,
		"z", &glm::vec3::z,
		sol::meta_function::addition, [](const glm::vec3& a, const glm::vec3& b) { return a + b; },
		sol::meta_function::subtraction, [](const glm::vec3& a, const glm::vec3& b) { return a - b; },
		"dot", [](const glm::vec3& a, const glm::vec3& b) { return glm::dot(a, b); },
		"cross", [](const glm::vec3& a, const glm::vec3& b) { return glm::cross(a, b); }
	);
	lua->set_function("radians", [](float degrees) {
		return glm::radians(degrees);
	});
	lua->set_function("cos", [](float degrees) {
		return glm::cos(degrees);
	});
	lua->set_function("sin", [](float degrees) {
		return glm::sin(degrees);
	});
}