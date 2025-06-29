#pragma once

#include "menuManager.h"

class MenuCreator {
public:
    MenuCreator(MenuManager& menuManager);

    void Render();

    void ToggleVisibility() { showMenuCreator = !showMenuCreator; }

    bool IsVisible() const { return showMenuCreator; }

private:
    MenuManager& menuManager;
    bool showMenuCreator = false; 
    MenuState menuType = MenuState::MAIN_MENU;
};
