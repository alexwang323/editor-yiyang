// [ginkgo-engine] transformcomponent.h : captures transforms

#pragma once

#include "ginkgopch.h"
#include <sol/sol.hpp>
#define GLM_ENABLE_EXPEREMENTAL
#include    <glm/gtx/matrix_cross_product.hpp>
#include    <glm/gtx/euler_angles.hpp>
#include    <glm/gtx/quaternion.hpp>
class TransformComponent {

friend class ComponentSerializer;
friend class collider;
public:
	TransformComponent() :
		position(glm::vec3(0.0f)),
		rotation_internal(glm::mat4(1.0f)),
		rotation(glm::vec3(0.0f)),
		scale(glm::vec3(1.0f)) { 
		getMatrix();
	}

	TransformComponent(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale) :
		position(_position),
		rotation_internal(glm::eulerAngleXYZ(_rotation.x, _rotation.y, _rotation.z)),
		rotation(_rotation),
		scale(_scale) {
		getMatrix();
	}


	void SetPosition(glm::vec3 _position);
	void SetRotation(glm::vec3 _rotation);
	void SetScale(glm::vec3 _scale);

	glm::vec3 GetPosition() const;
	glm::vec3 GetRotation() const;
	glm::vec3 GetScale() const;

	void ModPosition(glm::vec3 _position);
	void ModRotationEuler(glm::vec3 _rotation);
	//void ModRotation(glm::vec3 axis, float angle);
	void ModScale(glm::vec3 _scale);
	static void CreateTransformToLuaBind(std::shared_ptr<sol::state> lua);


	glm::mat4 getMatrix();

protected:
	glm::vec3 position;
	glm::mat4 rotation_internal = glm::mat4(1.0f);
	glm::vec3 rotation; // EULER
	glm::vec3 scale;
	bool needToUpdate = true;
	glm::mat4 matrix = glm::mat4(1.0f);
private:

};
