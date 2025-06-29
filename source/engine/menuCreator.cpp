#include "ginkgopch.h"
#include "menuCreator.h"
#include "imgui.h"
#include "consoleMessage.h"
#include <iostream>

MenuCreator::MenuCreator(MenuManager& menuManager)
    : menuManager(MenuManager::GetInstance()), menuType(MenuState::MAIN_MENU){}

void MenuCreator::Render() {
	//std::cout << showMenuCreator << std::endl;
    if (!showMenuCreator) return;

    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::OpenPopup("Settings Creator");

    if (ImGui::BeginPopupModal("Settings Creator", &showMenuCreator)) {

        // Select menu type
        const char* menuTypes[] = { "Main Menu", "In-Game Menu" };
        if (ImGui::BeginCombo("Menu Type", menuTypes[static_cast<int>(menuType)])) {
            if (ImGui::Selectable("Main Menu", menuType == MenuState::MAIN_MENU)) {
                menuType = MenuState::MAIN_MENU;
            }
            if (ImGui::Selectable("In-Game Menu", menuType == MenuState::IN_GAME_MENU)) {
                menuType = MenuState::IN_GAME_MENU;
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        // Add new menu item
        static char label[128] = "";
        static const char* actions[] = { "start_game", "open_settings", "exit_game", "resume_game", "back_to_main_menu" };
        static int actionIndex = 0;

        ImGui::Text("Add a New Menu Item");
        ImGui::InputText("Label", label, IM_ARRAYSIZE(label));
        ImGui::Combo("Action", &actionIndex, actions, IM_ARRAYSIZE(actions));

        if (ImGui::Button("Add Menu Item")) {
            if (std::strlen(label) == 0) {
                AddConsoleMessage("Label cannot be empty!");
            }
            else {
                if (menuType == MenuState::MAIN_MENU) {
                    MenuManager::GetInstance().GetMainMenuItems().push_back({ label, actions[actionIndex] });
                }
                else if (menuType == MenuState::IN_GAME_MENU) {
                    MenuManager::GetInstance().GetInGameMenuItems().push_back({ label, actions[actionIndex] });
                }
                strcpy_s(label, sizeof(label), "");
                AddConsoleMessage("Menu item added successfully.");
            }
        }

        ImGui::Separator();

        ImGui::Text("Current Menu Items:");

        std::vector<MenuItemData>* currentMenuItems = nullptr;

        if (menuType == MenuState::MAIN_MENU) {
            currentMenuItems = &MenuManager::GetInstance().GetMainMenuItems();
        }
        else if (menuType == MenuState::IN_GAME_MENU) {
            currentMenuItems = &MenuManager::GetInstance().GetInGameMenuItems();
        }

        // delete function
        for (int i = 0; i < currentMenuItems->size(); ++i) {
            ImGui::Text("Label: %s | Action: %s", (*currentMenuItems)[i].label.c_str(), (*currentMenuItems)[i].action.c_str());
            ImGui::SameLine();
            if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
                currentMenuItems->erase(currentMenuItems->begin() + i);
                AddConsoleMessage("Menu item deleted.");
            }
        }

        ImGui::Separator();

        // Save menu to file
        if (ImGui::Button("Save Menu")) {
            MenuManager::GetInstance().SaveMenus("menu.json");
            AddConsoleMessage("Menus saved successfully.");
        }

        ImGui::EndPopup();
    }
}
