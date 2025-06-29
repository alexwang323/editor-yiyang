#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "imgui.h"

struct MenuItemData {
    std::string label;
    std::string action;
};

enum class MenuState { 
    MAIN_MENU, 
    IN_GAME_MENU, 
	SETTINGS_MENU,
    NONE 
};

class MenuManager {
public:
    MenuManager(const MenuManager&) = delete;
    MenuManager& operator=(const MenuManager&) = delete;

    static MenuManager& GetInstance() {
        static MenuManager instance;
        return instance;
    }

    static void LoadMenus(const std::string& filename);
    static void SaveMenus(const std::string& filename);
    static void RenderActiveMenu();
    static void HandleMenuAction(const std::string& action);
	static void HandleBackAction();

    void SetCurrentMenuState(MenuState state) { currentMenuState = state; }
	void SetPreviousMenuState(MenuState state) { previousMenuState = state; }
	MenuState GetCurrentMenuState() const { return currentMenuState; }
	MenuState GetPreviousMenuState() const { return previousMenuState; }

	std::vector<MenuItemData>& GetMainMenuItems() { return mainMenuItems; }
	std::vector<MenuItemData>& GetInGameMenuItems() { return inGameMenuItems; }

	void SetWindow(GLFWwindow* window) { this->window = window; }

private:
    MenuManager() = default;
    ~MenuManager() = default;

    MenuState currentMenuState = MenuState::MAIN_MENU;
    MenuState previousMenuState = MenuState::MAIN_MENU;
    std::vector<MenuItemData> mainMenuItems;
    std::vector<MenuItemData> inGameMenuItems;

    GLFWwindow* window = nullptr;
};
