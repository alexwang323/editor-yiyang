#include "ginkgopch.h"
#include "sceneViewPanel.h"
#include "imgui.h"
#include "consoleMessage.h"
#include "myphysicsdirector.h"
#include "scene.h"
#include "Framebuffer.h"
#include "menuManager.h"

SceneViewPanel::SceneViewPanel()
    : scene(nullptr), framebuffer(nullptr), window(nullptr){

	isGameMenuVisible = true;
	firstMouse = true;
	lastX = 400.0f;
	lastY = 300.0f;
	wasFocusedLastFrame = false;
	isMouseLeftButtonClicked = false;

}

void SceneViewPanel::SetScene(Scene* scene) {
    this->scene = scene;
}

void SceneViewPanel::SetFramebuffer(Framebuffer* framebuffer) {
    this->framebuffer = framebuffer;
}

void SceneViewPanel::SetWindow(GLFWwindow* window) {
    this->window = window;
	MenuManager::GetInstance().SetWindow(window);
	SettingsManager::GetInstance().SetWindow(window);
}

void SceneViewPanel::Render(float deltaTime) {
    if (!scene) {
		AddConsoleMessage("Scene is not set.");
        return;
    }
	if (!framebuffer) {
		AddConsoleMessage("Framebuffer is not set.");
		return;
	}
	if (!window) {
		AddConsoleMessage("Window is not set.");
		return;
	}
    
    InputSystem& inputSystem = InputSystem::GetInstance();

    inputSystem.ProcessInput(deltaTime);
    Myphysicsdirector physicsDir = scene->GetPhysicsDirector();

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    if (MenuManager::GetInstance().GetCurrentMenuState() != MenuState::NONE) {
        MenuManager::GetInstance().RenderActiveMenu();
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(0, 50));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth * 0.666f, (float)(windowHeight - 300)));
    ImGui::Begin("Scene Viewport", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    int panelWidth = static_cast<int>(contentSize.x);
    int panelHeight = static_cast<int>(contentSize.y);

    if (panelWidth > 0 && panelHeight > 0 &&
        (framebuffer->GetWidth() != panelWidth || framebuffer->GetHeight() != panelHeight)) {
        framebuffer->Resize(panelWidth, panelHeight);
    }

    bool isSceneViewFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    inputSystem.SetSceneViewFocused(isSceneViewFocused);

    // Prevent camera movement when switching back to scene view
    if (isSceneViewFocused) {
        if (!wasFocusedLastFrame) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);

            firstMouse = false;
            inputSystem.ResetFirstMouse();
        }
    }
    wasFocusedLastFrame = isSceneViewFocused;

    if (panelWidth != 0 && panelHeight != 0) {
        framebuffer->Bind();
        glViewport(0, 0, panelWidth, panelHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef _DEBUG
        scene->Draw(*framebuffer, deltaTime);
#endif
        framebuffer->Unbind();
    }
    

	// Raycast to select objects TODO
    /*
    if (isSceneViewFocused && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!isMouseLeftButtonClicked) {
            isMouseLeftButtonClicked = true;

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            Camera* camera = scene->GetCamera();
            if (camera) {
                glm::vec3 rayOrigin;
                glm::vec3 rayDirection = CalculateRayStartAndDirection(mouseX, mouseY, windowWidth, windowHeight, camera, rayOrigin);

                collider* hitCollider = physicsDir._raycast(rayDirection, rayOrigin, 1000.0f);

                if (hitCollider) {
                    AddConsoleMessage("Ray hit an object!");
                }
                else {
                    AddConsoleMessage("Ray did not hit any objects.");
                }
            }
        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        isMouseLeftButtonClicked = false;
    }
    */

    ImGui::Image((void*)(intptr_t)framebuffer->GetTextureID(), contentSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

glm::vec3 SceneViewPanel::CalculateRayStartAndDirection(double mouseX, double mouseY, int screenWidth, int screenHeight, Camera* camera, glm::vec3& rayOrigin) {
    // Convert screen-space coordinates to NDC
    float x = (2.0f * static_cast<float>(mouseX)) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * static_cast<float>(mouseY)) / screenHeight;
    glm::vec4 rayNDC(x, y, -1.0f, 1.0f);

    // Transform ray from NDC to view space
    glm::vec4 rayView = glm::inverse(camera->ProjectionMatrix()) * rayNDC;
    rayView.z = -1.0f;
    rayView.w = 0.0f;

    // Transform ray from view space to world space
    glm::vec3 rayWorldDir = glm::normalize(glm::vec3(glm::inverse(camera->ViewMatrix()) * rayView));

    // Calculate the ray's origin
    glm::vec4 cameraWorldPos = glm::inverse(camera->ViewMatrix()) * glm::vec4(0, 0, 0, 1);
    rayOrigin = glm::vec3(cameraWorldPos);

    return rayWorldDir;
}