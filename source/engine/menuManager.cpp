#include "ginkgopch.h"
#include "menuManager.h"
#include "jsonmanager.h"
#include "settingsManager.h"
#include "consoleMessage.h"
#include <iostream>

void MenuManager::LoadMenus(const std::string& filename) {
    JSONmanager& json = JSONmanager::GetInstance();
    json.ResetReadStream(filename);

    auto mainMenuData = json.findValue("main_menu");
    if (mainMenuData && mainMenuData->get().IsArray()) {
        for (const auto& item : mainMenuData->get().GetArray()) {
            if (item.HasMember("label") && item["label"].IsString() &&
                item.HasMember("action") && item["action"].IsString()) {
                MenuManager::GetInstance().GetMainMenuItems().push_back({ item["label"].GetString(), item["action"].GetString() });
            }
        }
    }

    auto inGameMenuData = json.findValue("in_game_menu");
    if (inGameMenuData && inGameMenuData->get().IsArray()) {
        for (const auto& item : inGameMenuData->get().GetArray()) {
            if (item.HasMember("label") && item["label"].IsString() &&
                item.HasMember("action") && item["action"].IsString()) {
                MenuManager::GetInstance().GetInGameMenuItems().push_back({ item["label"].GetString(), item["action"].GetString() });
            }
        }
    }

    // default menu items if no menus were loaded
    if (MenuManager::GetInstance().GetMainMenuItems().empty()) {
        MenuManager::GetInstance().GetMainMenuItems() = {
            { "Start Game", "start_game" },
            { "Settings", "open_settings" },
            { "Exit Game", "exit_game" }
        };
    }

    if (MenuManager::GetInstance().GetInGameMenuItems().empty()) {
        MenuManager::GetInstance().GetInGameMenuItems() = {
            { "Resume Game", "resume_game" },
            { "Settings", "open_settings" },
            { "Back to Main Menu", "back_to_main_menu" }
        };
    }
}

void MenuManager::SaveMenus(const std::string& filename) {
    JSONmanager& json = JSONmanager::GetInstance();
    json.OpenWriteStream(filename);

    json.StartNewArray("main_menu");
    for (const auto& item : MenuManager::GetInstance().GetMainMenuItems()) {
        json.StartNewObject("");
        json.AddKeyValuePair("label", item.label);
        json.AddKeyValuePair("action", item.action);
        json.EndObject();
    }
    json.EndArray();

    json.StartNewArray("in_game_menu");
    for (const auto& item : MenuManager::GetInstance().GetInGameMenuItems()) {
        json.StartNewObject("");
        json.AddKeyValuePair("label", item.label);
        json.AddKeyValuePair("action", item.action);
        json.EndObject();
    }
    json.EndArray();
    json.EndWriteStream();
}

void MenuManager::RenderActiveMenu() {
	//std::cout << static_cast<int>(MenuManager::GetInstance().GetCurrentMenuState()) << std::endl;
    if (MenuManager::GetInstance().GetCurrentMenuState() == MenuState::SETTINGS_MENU) {
        SettingsManager::GetInstance().RenderSettingsMenu();
        return;
    }

    const std::vector<MenuItemData>* menuItems = nullptr;
	
    if (MenuManager::GetInstance().GetCurrentMenuState() == MenuState::MAIN_MENU) {
        menuItems = &MenuManager::GetInstance().GetMainMenuItems();
    }
    else if (MenuManager::GetInstance().GetCurrentMenuState() == MenuState::IN_GAME_MENU) {
        menuItems = &MenuManager::GetInstance().GetInGameMenuItems();
    }
    else {
        return; // No active menu to render
    }

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    int windowWidth, windowHeight;
    if (MenuManager::GetInstance().window) {
#ifdef _DEBUG
        glfwGetFramebufferSize(MenuManager::GetInstance().window, &windowWidth, &windowHeight);
        windowWidth = static_cast<int>(windowWidth * 0.666f);
		windowHeight -= 300;
        ImGui::SetNextWindowPos(ImVec2(0, 50));
#else
		glfwGetWindowSize(MenuManager::GetInstance().window, &windowWidth, &windowHeight);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
#endif
	}
	else {
		windowWidth = 1200;
		windowHeight = 800;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
		AddConsoleMessage("Window is not set in MenuManager.");
	}
    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)windowHeight));
    ImGui::Begin("Game Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));

    for (const auto& item : *menuItems) {
        if (ImGui::Selectable(item.label.c_str(), false, ImGuiSelectableFlags_None, ImVec2(0, 50))) {
            HandleMenuAction(item.action);
        }
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void MenuManager::HandleMenuAction(const std::string& action) {
    if (action == "start_game") {
        MenuManager::GetInstance().SetPreviousMenuState(MenuManager::GetInstance().GetCurrentMenuState());
        MenuManager::GetInstance().SetCurrentMenuState(MenuState::NONE);
    }
    else if (action == "open_settings") {
        MenuManager::GetInstance().SetPreviousMenuState(MenuManager::GetInstance().GetCurrentMenuState());
		MenuManager::GetInstance().SetCurrentMenuState(MenuState::SETTINGS_MENU);
    }
    else if (action == "exit_game") {
        exit(0);
    }
    else if (action == "resume_game") {
        MenuManager::GetInstance().SetPreviousMenuState(MenuManager::GetInstance().GetCurrentMenuState());
        MenuManager::GetInstance().SetCurrentMenuState(MenuState::NONE);
    }
    else if (action == "back_to_main_menu") {
        MenuManager::GetInstance().SetPreviousMenuState(MenuManager::GetInstance().GetCurrentMenuState());
        MenuManager::GetInstance().SetCurrentMenuState(MenuState::MAIN_MENU);
    }
}

void MenuManager::HandleBackAction() {
	if (MenuManager::GetInstance().GetCurrentMenuState() == MenuState::SETTINGS_MENU) {
		MenuManager::GetInstance().SetCurrentMenuState(MenuManager::GetInstance().GetPreviousMenuState());
	}
	else if (MenuManager::GetInstance().GetCurrentMenuState() == MenuState::IN_GAME_MENU) {
		MenuManager::GetInstance().SetCurrentMenuState(MenuState::NONE);
	}
}