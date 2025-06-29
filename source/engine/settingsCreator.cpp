#include "ginkgopch.h"
#include "settingsCreator.h"
#include "imgui.h"
#include "consoleMessage.h"
#include <filesystem>

SettingsCreator::SettingsCreator() {
	//JSONmanager::GetInstance().Init("settings_config.json");
    //JSONmanager::GetInstance().Init("ettings_game.json");
    SettingsManager::GetInstance().LoadSettingsConfig("settings_config.json");
	SettingsManager::GetInstance().LoadSettings("settings_game.json");
	AddConsoleMessage("Loaded settings configs.");
}

void SettingsCreator::Render() {
    //std::cout << "Setting Creator visible: " << showSettingsCreator << std::endl;
    if (!showSettingsCreator) return;

    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImGui::OpenPopup("Settings Creator");
    if (ImGui::BeginPopupModal("Settings Creator", &showSettingsCreator)) {
        if (ImGui::CollapsingHeader("Categories", ImGuiTreeNodeFlags_DefaultOpen)) {
            RenderCategoryEditor();
            ImGui::Separator();

            for (auto& category : SettingsManager::GetInstance().GetCategories()) {
                if (ImGui::TreeNode(category.name.c_str())) {
                    RenderSettingEditor(category);
                    ImGui::TreePop();
                }
            }
        }

        if (ImGui::Button("Save Settings Config")) {
            SettingsManager::SaveSettingsConfig("settings_config.json");
        }
        ImGui::EndPopup();
    }
}

void SettingsCreator::RenderCategoryEditor() {
    ImGui::InputText("Category Name", categoryName, sizeof(categoryName));
    ImGui::InputText("Category ID", categoryId, sizeof(categoryId));

    if (ImGui::Button("Add Category")) {
        if (strlen(categoryName) > 0 && strlen(categoryId) > 0) {
            SettingsManager::GetInstance().GetCategories().push_back({ categoryName, categoryId });
            memset(categoryName, 0, sizeof(categoryName));
            memset(categoryId, 0, sizeof(categoryId));
            AddConsoleMessage("Added new settings category.");
        }
    }

    ImGui::Separator();

    for (auto it = SettingsManager::GetInstance().GetCategories().begin();
        it != SettingsManager::GetInstance().GetCategories().end();) {
        ImGui::Text("%s (%s)", it->name.c_str(), it->id.c_str());
        ImGui::SameLine();
        if (ImGui::Button(("Delete##" + it->id).c_str())) {
            for (const auto& setting : it->settings) {
                SettingsManager::GetInstance().RemoveValue(setting.id);
                SettingsManager::GetInstance().RemoveCallback(setting.id);
            }
            it = SettingsManager::GetInstance().GetCategories().erase(it);
            AddConsoleMessage("Category deleted.");
            continue;
        }
        ++it;
    }
}

void SettingsCreator::RenderSettingEditor(SettingCategory& category) {
    ImGui::InputText("Setting Label", settingLabel, sizeof(settingLabel));
    ImGui::InputText("Setting ID", settingId, sizeof(settingId));
    ImGui::Combo("Type", &settingType, "Slider\0Checkbox\0Dropdown\0");

    if (settingType == 0) { // Slider
        ImGui::InputFloat("Min Value", &minValue);
        ImGui::InputFloat("Max Value", &maxValue);
    }
    else if (settingType == 2) { // Dropdown
        ImGui::InputText("Options (comma separated)", options, sizeof(options));
    }

    if (ImGui::Button("Add Setting")) {
        if (strlen(settingLabel) > 0 && strlen(settingId) > 0) {
            SettingConfig config;
            config.label = settingLabel;
            config.id = settingId;
            config.type = static_cast<SettingType>(settingType);

            if (settingType == 0) { // Slider
                config.minValue = minValue;
                config.maxValue = maxValue;
                config.defaultValue = minValue;
                SettingsManager::GetInstance().GetValues()[config.id] = minValue;
            }
            else if (settingType == 1) { // Checkbox
                config.defaultValue = false;
                SettingsManager::GetInstance().GetValues()[config.id] = false;
            }
            else if (settingType == 2) { // Dropdown
                std::string optionsStr(options);
                size_t pos = 0;
                while ((pos = optionsStr.find(',')) != std::string::npos) {
                    config.options.push_back(optionsStr.substr(0, pos));
                    optionsStr.erase(0, pos + 1);
                }
                if (!optionsStr.empty()) {
                    config.options.push_back(optionsStr);
                }
                config.defaultValue = 0;
                SettingsManager::GetInstance().GetValues()[config.id] = 0;
            }

            category.settings.push_back(config);
            memset(settingLabel, 0, sizeof(settingLabel));
            memset(settingId, 0, sizeof(settingId));
            memset(options, 0, sizeof(options));
            AddConsoleMessage("Added new setting to category.");
        }
    }

    for (auto it = category.settings.begin(); it != category.settings.end();) {
        ImGui::Text("%s (%s)", it->label.c_str(), it->id.c_str());
        ImGui::SameLine();
        if (ImGui::Button(("Delete##" + it->id).c_str())) {
            it = category.settings.erase(it);
            AddConsoleMessage("Deleted setting.");
        }
        else {
            ++it;
        }
    }
}