#pragma once

#include "imgui.h"
#include "menuCreator.h"
#include "menuManager.h"
#include "toolbar.h"
#include "sceneViewPanel.h"
#include "inspectorPanel.h"
#include "hierarchyPanel.h"
#include "consolePanel.h"
#include "settingsCreator.h"

class Scene;
class Framebuffer;
struct GLFWwindow;

class Editor {
public:
    Editor();

    void Init(Scene& scene, Framebuffer& framebuffer, GLFWwindow* window);

    void Render(float deltaTime);

private:
    Scene* scene;
    Framebuffer* framebuffer;
    GLFWwindow* window;

    MenuManager& menuManager;
    MenuCreator menuCreator;
	SettingsCreator settingsCreator;

    Toolbar toolbar;
    SceneViewPanel sceneViewPanel;
	InspectorPanel inspectorPanel;
	HierarchyPanel hierarchyPanel;
	ConsolePanel consolePanel;
};