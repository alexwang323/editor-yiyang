#include "ginkgopch.h"
#include "transformcomponent.h"

void TransformComponent::SetPosition(glm::vec3 _position) {
	position = _position;
	needToUpdate = true;
}

void TransformComponent::SetRotation(glm::vec3 _rotation)
{
	//rotation_internal
	rotation = _rotation;
	rotation_internal = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	needToUpdate = true;
}

void TransformComponent::SetScale(glm::vec3 _scale) {
	scale = _scale;
	needToUpdate = true;
}

void TransformComponent::ModPosition(glm::vec3 _position) {
	position += _position;
	needToUpdate = true;
}

void TransformComponent::ModRotationEuler(glm::vec3 _rotation)
{
	rotation += _rotation;
	rotation_internal = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	needToUpdate = true;
}


glm::vec3 TransformComponent::GetPosition() const{
	return position;
}
glm::vec3 TransformComponent::GetRotation() const{
	return rotation;
}
glm::vec3 TransformComponent::GetScale() const{
	return scale;
}

//void TransformComponent::ModRotation(glm::vec3 axis, float angle) {
//	needToUpdate = true;
//}

void TransformComponent::ModScale(glm::vec3 _scale) {
	scale += _scale;
	needToUpdate = true;
}

glm::mat4 TransformComponent::getMatrix() {
	// TODO: move to update()
	if (needToUpdate) {

		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0), scale);
		//glm::mat4 rotationMat = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
		glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), position);

		matrix = translateMat * rotation_internal * scaleMat;
		needToUpdate = false;
	}

	return matrix;

}

void TransformComponent::CreateTransformToLuaBind(std::shared_ptr<sol::state> lua) {
	lua->new_usertype<TransformComponent>("TransformComponent",
		sol::constructors<TransformComponent(),
		TransformComponent(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale),
		std::shared_ptr<TransformComponent>(glm::vec3 _position, glm::vec3 _rotation, glm::vec3 _scale)>(),
		"position", &TransformComponent::position,
		"rotation", &TransformComponent::rotation,
		"scale", &TransformComponent::scale,
		"needToUpdate", &TransformComponent::needToUpdate,
		"SetPosition", &TransformComponent::SetPosition,
		"SetRotation", &TransformComponent::SetRotation,
		"SetScale", &TransformComponent::SetScale,
		"ModScale", &TransformComponent::ModScale,
		"ModPosition", &TransformComponent::ModPosition,
		"Rotate", &TransformComponent::ModRotationEuler
	);

}
