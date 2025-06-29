#include "ginkgopch.h"
#include "toolbar.h"
#include "imgui.h"
#include "consoleMessage.h"
#include <iostream>

Toolbar::Toolbar(MenuCreator& menuCreator, SettingsCreator& settingsCreator)
    : menuCreator(menuCreator), settingsCreator(settingsCreator) {}

void Toolbar::SetScene(Scene* scene) {
	this->scene = scene;
}

void Toolbar::SetDrawMode(DrawModes drawMode) {
	if (!scene) {
		AddConsoleMessage("Scene is not set in toolbar!");
        return;
	}
    switch (drawMode) {
    case DrawModes::MESH_NORMAL:
		scene->setMeshDrawMode(MESH_DRAW_MODE::MESH_DRAW_MODE_NORMAL);
		currentMeshDrawMode = MESH_DRAW_MODE::MESH_DRAW_MODE_NORMAL;
        AddConsoleMessage("Draw mode: MESH_NORMAL");
        break;

    case DrawModes::MESH_WIREFRAME:
		scene->setMeshDrawMode(MESH_DRAW_MODE::MESH_DRAW_MODE_WIREFRAME);
		currentMeshDrawMode = MESH_DRAW_MODE::MESH_DRAW_MODE_WIREFRAME;
        AddConsoleMessage("Draw mode: MESH_WIREFRAME");
        break;

    case DrawModes::SCENE_DEFERRED_MSAA:
		scene->setSceneDrawMode(SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEFERRED_MSAA);
		currentSceneDrawMode = SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEFERRED_MSAA;
        AddConsoleMessage("Draw mode: SCENE_DEFERRED_MSAA");
        break;

    case DrawModes::SCENE_NO_DEFERRED:
		scene->setSceneDrawMode(SCENE_DRAW_MODE::SCENE_DRAW_MODE_NO_DEFERRED);
		currentSceneDrawMode = SCENE_DRAW_MODE::SCENE_DRAW_MODE_NO_DEFERRED;
        AddConsoleMessage("Draw mode: SCENE_NO_DEFERRED");
        break;

    case DrawModes::SCENE_DEBUG_QUAD:
		scene->setSceneDrawMode(SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_QUAD);
		currentSceneDrawMode = SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_QUAD;
        AddConsoleMessage("Draw mode: SCENE_DEBUG_QUAD");
        break;

    case DrawModes::SCENE_DEBUG_QUAD_G_NRM:
		scene->setSceneDebugDisplay(SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_G_NRM);
		currentSceneDebugDisplay = SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_G_NRM;
        AddConsoleMessage("Draw mode: SCENE_DEBUG_QUAD_G_NRM");
        break;

    case DrawModes::SCENE_DEBUG_QUAD_SHADOW:
		scene->setSceneDebugDisplay(SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_SHADOW);
		currentSceneDebugDisplay = SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_SHADOW;
        AddConsoleMessage("Draw mode: SCENE_DEBUG_QUAD_SHADOW");
        break;

    case DrawModes::SCENE_DEBUG_STENCIL:
		scene->setSceneDrawMode(SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_STENCIL);
		currentSceneDrawMode = SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_STENCIL;
        AddConsoleMessage("Draw mode: SCENE_DRAW_MODE_DEBUG_STENCIL");
        break;

    default:
        AddConsoleMessage("Unknown Draw Mode!");
        break;
    }
}

void Toolbar::Render(int windowWidth) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, 50.0f));
    ImGui::Begin("Toolbar", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	/*
    if (ImGui::Button("Play")) {
        AddConsoleMessage("Play button clicked!");
    }
    ImGui::SameLine();

    if (ImGui::Button("Pause")) {
        AddConsoleMessage("Pause button clicked!");
    }
    ImGui::SameLine();

    if (ImGui::Button("Stop")) {
        AddConsoleMessage("Stop button clicked!");
    }
    ImGui::SameLine();
	*/
    if (ImGui::Button("Create Menu")) {
        menuCreator.ToggleVisibility();
        //std::cout << menuCreator.IsVisible() << std::endl;
    }
    ImGui::SameLine();

    if (ImGui::Button("Settings Editor")) {
        settingsCreator.ToggleVisibility();
		std::cout << settingsCreator.IsVisible() << std::endl;
    }
    
	// combobox for draw modes
	float width = 140.0f;
	ImGui::PushItemWidth(width);
    {
        const char* meshModes[] = { "Normal", "Wireframe" };
		int meshModeIndex = 0;
		switch (currentMeshDrawMode) {
		case MESH_DRAW_MODE::MESH_DRAW_MODE_NORMAL:
			meshModeIndex = 0;
			break;
		case MESH_DRAW_MODE::MESH_DRAW_MODE_WIREFRAME:
			meshModeIndex = 1;
			break;
		}

        ImGui::SameLine();
        ImGui::Text("Mesh Draw Mode:");
        ImGui::SameLine();
        if (ImGui::Combo("##MeshMode", &meshModeIndex, meshModes, IM_ARRAYSIZE(meshModes))) {
			switch (meshModeIndex)
			{
			case 0:
				SetDrawMode(DrawModes::MESH_NORMAL);
				break;
			case 1:
				SetDrawMode(DrawModes::MESH_WIREFRAME);
				break;
			}
        }
	}
	ImGui::PopItemWidth();
	ImGui::PushItemWidth(width);
	{
		const char* sceneModes[] = { "Deferred MSAA", "No Deferred", "Debug Quad", "Debug Stencil" };
		int sceneModeIndex = 0;
		switch (currentSceneDrawMode) {
		case SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEFERRED_MSAA:
			sceneModeIndex = 0;
			break;
		case SCENE_DRAW_MODE::SCENE_DRAW_MODE_NO_DEFERRED:
			sceneModeIndex = 1;
			break;
		case SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_QUAD:
			sceneModeIndex = 2;
			break;
		case SCENE_DRAW_MODE::SCENE_DRAW_MODE_DEBUG_STENCIL:
			sceneModeIndex = 3;
			break;
		}

		ImGui::SameLine();
		ImGui::Text("Scene Draw Mode:");
		ImGui::SameLine();
		if (ImGui::Combo("##SceneMode", &sceneModeIndex, sceneModes, IM_ARRAYSIZE(sceneModes))) {
			switch (sceneModeIndex) {
			case 0:
				SetDrawMode(DrawModes::SCENE_DEFERRED_MSAA);
				showSceneDebugDisplay = false;
				break;
			case 1:
				SetDrawMode(DrawModes::SCENE_NO_DEFERRED);
				showSceneDebugDisplay = false;
				break;
			case 2:
				SetDrawMode(DrawModes::SCENE_DEBUG_QUAD);
				showSceneDebugDisplay = true;
				break;
			case 3:
				SetDrawMode(DrawModes::SCENE_DEBUG_STENCIL);
				showSceneDebugDisplay = false;
				break;
			}
		}
	}
	ImGui::PopItemWidth();

	if (showSceneDebugDisplay) {
		ImGui::PushItemWidth(width);
		{
			const char* sceneDebugDisplays[] = { "G Normal", "Shadow" };
			int sceneDebugDisplayIndex = 0;
			switch (currentSceneDebugDisplay) {
			case SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_G_NRM:
				sceneDebugDisplayIndex = 0;
				break;
			case SCENE_DEBUG_DISPLAY::SCENE_DEBUG_DISPLAY_SHADOW:
				sceneDebugDisplayIndex = 1;
				break;
			}

			ImGui::SameLine();
			ImGui::Text("Scene Debug Display:");
			ImGui::SameLine();
			if (ImGui::Combo("##SceneDebugDisplay", &sceneDebugDisplayIndex, sceneDebugDisplays, IM_ARRAYSIZE(sceneDebugDisplays))) {
				switch (sceneDebugDisplayIndex) {
				case 0:
					SetDrawMode(DrawModes::SCENE_DEBUG_QUAD_G_NRM);
					break;
				case 1:
					SetDrawMode(DrawModes::SCENE_DEBUG_QUAD_SHADOW);
					break;
				}
			}
		}
		ImGui::PopItemWidth();
	}

	
	static bool isGameMode = false;
	ImGui::SameLine();
	if (ImGui::Checkbox("Game Mode", &isGameMode)) {
		if (scene) {
			if (inputSystem) {
				inputSystem->SetMode(isGameMode ? InputMode::GAME : InputMode::EDITOR);
				AddConsoleMessage(std::string("Input Mode: ") + (isGameMode ? "GAME" : "EDITOR"));
			}
			else {
				AddConsoleMessage("InputSystem is not set!");
			}
		}
		else {
			AddConsoleMessage("Scene is not set!");
		}
	}
	ImGui::SameLine();

	if (ImGui::Checkbox("Show Debug", &showDebugWindows)) {
		AddConsoleMessage(std::string("Debug window ") + (showDebugWindows ? "enabled" : "disabled"));
	}

    ImGui::End();
}
