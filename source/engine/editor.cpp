#include "ginkgopch.h"
#include "editor.h"
#include "hierarchyPanel.h"
#include "scene.h"
#include "Framebuffer.h"
#include "InputSystem.h"

int selectedObjectID = -1;

Editor::Editor()
    : menuManager(MenuManager::GetInstance()), 
    menuCreator(menuManager),
    settingsCreator(),
    toolbar(menuCreator, settingsCreator) {}

void Editor::Init(Scene& scene, Framebuffer& framebuffer, GLFWwindow* window) {
    this->scene = &scene;
    this->framebuffer = &framebuffer;
    this->window = window;

    InputSystem& inputSystem = InputSystem::GetInstance();
    inputSystem.Initialize(window, InputMode::EDITOR);

    inputSystem.SetCamera(scene.GetCamera());

    sceneViewPanel.SetScene(&scene);
    sceneViewPanel.SetFramebuffer(&framebuffer);
	sceneViewPanel.SetWindow(window);
    inputSystem.RegisterCallbacks();
	inspectorPanel.SetScene(&scene);
	hierarchyPanel.SetScene(&scene);
	consolePanel.SetScene(&scene);
	toolbar.SetScene(&scene);
	toolbar.setinputSystem(&inputSystem);
}

void Editor::Render(float deltaTime) {
	// Get window size for dynamic resizing
    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

    InputSystem& inputSystem = InputSystem::GetInstance();

	toolbar.Render(windowWidth);
    menuCreator.Render();
    settingsCreator.Render();
    sceneViewPanel.Render(deltaTime);
	hierarchyPanel.Render(selectedObjectID, windowWidth, windowHeight);
	inspectorPanel.Render(selectedObjectID, windowWidth, windowHeight);
	consolePanel.Render(windowWidth, windowHeight);

    if (toolbar.GetShowDebugWindows()) {
        inputSystem.DisplayKeyMappingDebug();
        inputSystem.DisplayInputAssignmentUI();
    }
}