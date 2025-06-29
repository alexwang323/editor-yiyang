#pragma once

#include "menuManager.h"
#include "menuCreator.h"
#include "settingsCreator.h"
#include "scene.h"
#include "mesh.h"
#include "InputSystem.h"

enum class DrawModes {
	MESH_NORMAL,
	MESH_WIREFRAME,

	SCENE_DEFERRED_MSAA,
	SCENE_NO_DEFERRED,
	SCENE_DEBUG_QUAD,
	SCENE_DEBUG_STENCIL,

	SCENE_DEBUG_QUAD_G_NRM,
	SCENE_DEBUG_QUAD_SHADOW
};

class Toolbar {
public:
    Toolbar(MenuCreator& menuCreator, SettingsCreator& settingsCreator);

    void Render(int windowWidth);
    void SetScene(Scene* scene);
	void SetDrawMode(DrawModes drawMode);
	void setinputSystem(InputSystem* input) { inputSystem = input; }
	bool GetShowDebugWindows() const { return showDebugWindows; }

private:
    MenuCreator& menuCreator;
    SettingsCreator& settingsCreator;
	Scene* scene;
	InputSystem* inputSystem;

	MESH_DRAW_MODE currentMeshDrawMode = MESH_DRAW_MODE::MESH_DRAW_MODE_NORMAL;
	SCENE_DRAW_MODE currentSceneDrawMode = SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEFERRED_MSAA;
	SCENE_DEBUG_DISPLAY currentSceneDebugDisplay = SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_G_NRM;

	bool showSceneDebugDisplay = false;
	bool showDebugWindows = false;
};
