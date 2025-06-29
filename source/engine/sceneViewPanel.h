#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "camera.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "InputSystem.h"

class Framebuffer;
class Scene;

class SceneViewPanel {
public:
    SceneViewPanel();

    void Render(float deltaTime);
    void SetScene(Scene* scene);
    void SetFramebuffer(Framebuffer* framebuffer);
    void SetWindow(GLFWwindow* window);

private:
    //void ProcessInput(float deltaTime);
    glm::vec3 CalculateRayStartAndDirection(double mouseX, double mouseY, int screenWidth, int screenHeight, Camera* camera, glm::vec3& rayOrigin);

    Scene* scene;
    Framebuffer* framebuffer;
    GLFWwindow* window;

    bool isGameMenuVisible;
    bool firstMouse;
    float lastX, lastY;

    bool wasFocusedLastFrame;
    bool isMouseLeftButtonClicked;
};