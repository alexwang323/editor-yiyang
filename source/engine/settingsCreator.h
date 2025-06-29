#pragma once
#include "settingsManager.h"

class SettingsCreator {
public:
    SettingsCreator();

    void Render();
    void ToggleVisibility() { showSettingsCreator = !showSettingsCreator; }
    bool IsVisible() const { return showSettingsCreator; }

private:
    void RenderCategoryEditor();
    void RenderSettingEditor(SettingCategory& category);

    bool showSettingsCreator = false;
    char categoryName[128] = "";
    char categoryId[128] = "";
    char settingLabel[128] = "";
    char settingId[128] = "";
    char options[256] = "";
    int settingType = 0;
    float minValue = 0.0f;
    float maxValue = 1.0f;
};