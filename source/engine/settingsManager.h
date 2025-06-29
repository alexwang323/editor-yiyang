#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "settingsType.h"
#include "jsonmanager.h"
#include "menuManager.h"
#include <unordered_map>

class SettingsManager {
public:
    static SettingsManager& GetInstance() {
        static SettingsManager instance;
        return instance;
    }

    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    void RenderSettingsMenu();
    static void LoadSettingsConfig(const std::string& filename);
    static void SaveSettingsConfig(const std::string& filename);

	void setTempValue(const std::string& id, const SettingValue& value) { tempValues[id] = value; }
    void SetValue(const std::string& id, const SettingValue& value);
    std::variant<float, bool, int> GetValue(const std::string& id) const;

    void LoadSettings(const std::string& filename);
    void SaveSettings(const std::string& filename) const;

    std::vector<SettingCategory>& GetCategories() { return categories; }
    const std::vector<SettingCategory>& GetCategories() const { return categories; }

    void RegisterCallback(const std::string& settingId, SettingCallback callback);
    void RemoveCallback(const std::string& settingId) { callbacks.erase(settingId); }
	void RemoveValue(const std::string& settingId) { values.erase(settingId); }

	void SetWindow(GLFWwindow* window) { this->window = window; }
	std::unordered_map<std::string, SettingValue>& GetValues() { return values; }

    bool IsRightPanelFocused() const { return isRightPanelFocused; }
	void SetRightPanelFocused(bool value) { isRightPanelFocused = value; }

private:
    SettingsManager() = default;
    ~SettingsManager() = default;

    void RenderSettingItem(const SettingConfig& config);

    std::vector<SettingCategory> categories;
    std::string currentCategory;

    std::unordered_map<std::string, SettingCallback> callbacks;
    std::unordered_map<std::string, SettingValue> values;
	std::unordered_map<std::string, SettingValue> tempValues;

	GLFWwindow* window;
	bool isRightPanelFocused = false;
};