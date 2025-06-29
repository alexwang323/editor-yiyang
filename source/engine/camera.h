// [ginkgo-engine] camera.h : manager for view matrix and associated parameters

#pragma once

#include "ginkgopch.h"

class Camera {

public:
    // TODO: calculate a sensible zoom default value
    Camera(glm::vec3 _position = glm::vec3(0.0f), float _yaw = 0.0f, float _pitch = 0.0f,
        float _fov = 45.0f, float _aspectRatio = 16.0f / 9.0f, float _nearPlane = 0.1f, float _farPlane = 1000.0f)
        : position(_position), yaw(_yaw), pitch(_pitch), fov(_fov), aspectRatio(_aspectRatio), nearPlane(_nearPlane), farPlane(_farPlane)
    {
        UpdateMatrix();
        UpdateProjectionMatrix();
    }

    // TODO: how does this work with player?

    glm::vec3 position{ glm::vec3(0.0f) };
    float yaw=0.0f, pitch=0.0f; // RADIANS
    float zoom=25.0f;

    // Unused. Possibly useful if we move position/yaw/pitch/zoom to private.
    void TranslateCamera(glm::vec3 translation);
    void RotateCamera(float xRotation, float yRotation);

    // NOTE: in current state (public params), 
    // this MUST be called after translation/rotation.
    void UpdateMatrix();
    void UpdateProjectionMatrix(float newFov = -1.0f, float newAspectRatio = -1.0f);

    glm::mat4 ViewMatrix() const { return view; }
    glm::mat4 ProjectionMatrix() const { return projection; }

    glm::vec3 GetFront() const { return front; }
    //void SetSensitivity(float value) { sensitivity = value; }
	glm::vec3 GetPosition() const { return position; }

    void UpdateDirection();

private:

    //float sensitivity = 0.1;

    glm::vec3 front{ glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };
    glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
    glm::vec3 right{ glm::vec3(1.0f, 0.0f, 0.0f) };
    glm::mat4 view{ glm::mat4(1.0f) }; // named "view" for consistency with MVP matrix

    float fov = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    glm::mat4 projection{ glm::mat4(1.0f) };
};
