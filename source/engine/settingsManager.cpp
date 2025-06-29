#include "ginkgopch.h"
#include "settingsManager.h"
#include "imgui.h"
#include "consoleMessage.h"
#include "jsonmanager.h"
#include "menuManager.h"
#include <filesystem>

void SettingsManager::SetValue(const std::string& id, const SettingValue& value) {
    values[id] = value;

    auto it = callbacks.find(id);
    if (it != callbacks.end()) {
        it->second(value);
    }

    SaveSettings("settings_game.json");
}

std::variant<float, bool, int> SettingsManager::GetValue(const std::string& id) const {
    auto it = values.find(id);

    if (it != values.end()) {
        return it->second;
    }

    return 0.0f;
}

void SettingsManager::LoadSettingsConfig(const std::string& filename) {
    try {
        JSONmanager& json = JSONmanager::GetInstance();
        // if config file doesn't exist, create an empty one and return
        if (!std::filesystem::exists(filename)) {
            std::ofstream file(filename);
            file << "{}" << std::endl;
            file.close();
            return;
        }

        // initialize JSON
        if (!json.Init(filename)) {
            throw std::runtime_error("Failed to initilize JSON manager");
        }

        auto settingsData = json.findValue("settings_config");
        if (!settingsData || !settingsData->get().IsArray()) return;

        // clear any previously loaded categories
        GetInstance().categories.clear();

		// Parse categories
        for (const auto& categoryData : settingsData->get().GetArray()) {
            SettingCategory category;
            category.name = categoryData["name"].GetString();
            category.id = categoryData["id"].GetString();

            // Parse settings
            if (categoryData.HasMember("settings") && categoryData["settings"].IsArray()) {
                for (const auto& settingData : categoryData["settings"].GetArray()) {
                    SettingConfig config;
                    config.label = settingData["label"].GetString();
                    config.id = settingData["id"].GetString();
                    config.type = static_cast<SettingType>(settingData["type"].GetInt());

                    // min/max for sliders
                    if (settingData.HasMember("min")) config.minValue = settingData["min"].GetFloat();
                    if (settingData.HasMember("max")) config.maxValue = settingData["max"].GetFloat();

                    // options for dropdown
                    if (settingData.HasMember("options") && settingData["options"].IsArray()) {
                        for (const auto& optionObj : settingData["options"].GetArray()) {
                            if (optionObj.HasMember("value") && optionObj["value"].IsString()) {
                                config.options.push_back(optionObj["value"].GetString());
                            }
                        }
                    }

					// use default value if not set
                    auto& values = GetInstance().values;
                    if (values.find(config.id) == values.end() && settingData.HasMember("default")) {
                        switch (config.type) {
                        case SettingType::SLIDER:
                            if (settingData["default"].IsNumber()) {
                                config.defaultValue = settingData["default"].GetFloat();
                                values[config.id] = settingData["default"].GetFloat();
                            }
                            break;
                        case SettingType::CHECKBOX:
                            if (settingData["default"].IsBool()) {
                                config.defaultValue = settingData["default"].GetBool();
                                values[config.id] = settingData["default"].GetBool();
                            }
                            break;
                        case SettingType::DROPDOWN:
                            if (settingData["default"].IsNumber()) {
                                int value = settingData["default"].IsInt() ?
                                    settingData["default"].GetInt() :
                                    static_cast<int>(settingData["default"].GetFloat());
                                config.defaultValue = value;
                                values[config.id] = value;
                            }
                            break;
                        }
                    }

                    category.settings.push_back(config);
                }
            }
            GetInstance().categories.push_back(category);
        }
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Failed to load settings config: " << e.what() << std::endl;
        AddConsoleMessage("Failed to load settings config.");
    }
}

void SettingsManager::SaveSettingsConfig(const std::string& filename) {
    try {
        // make sure parent directory exists
        std::filesystem::path filePath(filename);
        if (filePath.has_parent_path()) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        JSONmanager& json = JSONmanager::GetInstance();
        if (!json.OpenWriteStream(filename)) {
            throw std::runtime_error("Failed to open write stream");
            return;
        }

        // Writing
        json.StartNewArray("settings_config");

        // Serializing
        for (const auto& category : GetInstance().categories) {
            json.StartNewObject("");
            json.AddKeyValuePair("name", category.name);
            json.AddKeyValuePair("id", category.id);

            json.StartNewArray("settings");
            for (const auto& setting : category.settings) {
                json.StartNewObject("");
                json.AddKeyValuePair("label", setting.label);
                json.AddKeyValuePair("id", setting.id);
                json.AddKeyValuePair("type", static_cast<int>(setting.type));

				// based on setting type adjust the value
                switch (setting.type) {
                case SettingType::SLIDER:
                    json.AddKeyValuePair("min", setting.minValue);
                    json.AddKeyValuePair("max", setting.maxValue);
                    if (std::holds_alternative<float>(setting.defaultValue)) {
                        json.AddKeyValuePair("default", std::get<float>(setting.defaultValue));
                    }
                    else {
                        json.AddKeyValuePair("default", 0.0f);
                    }
                    break;

                case SettingType::CHECKBOX:
                    if (std::holds_alternative<bool>(setting.defaultValue)) {
                        json.AddKeyValuePair("default", std::get<bool>(setting.defaultValue));
                    }
                    else {
                        json.AddKeyValuePair("default", false);
                    }
                    break;

                case SettingType::DROPDOWN:
                    json.StartNewArray("options");
                    for (const auto& option : setting.options) {
                        json.StartNewObject("");
                        json.AddKeyValuePair("value", option);
                        json.EndObject();
                    }
                    json.EndArray();
                    if (std::holds_alternative<int>(setting.defaultValue)) {
                        json.AddKeyValuePair("default", std::get<int>(setting.defaultValue));
                    }
                    else {
                        json.AddKeyValuePair("default", 0);
                    }
                    break;
                }

                json.EndObject();
            }
            json.EndArray();
            json.EndObject();
        }

        json.EndArray();
        json.EndWriteStream();

        AddConsoleMessage("Settings configuration saved successfully.");
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save settings config: " << e.what() << std::endl;
        AddConsoleMessage("Failed to save settings configuration");
    }
}

void SettingsManager::LoadSettings(const std::string& filename) {
    try {
        // If file does not exist creat emply settings JSON
        if (!std::filesystem::exists(filename)) {
            std::ofstream file(filename);
            file << "{\"settings\":{}}" << std::endl;
            file.close();
            AddConsoleMessage("Created new settings file.");
            return;
        }

        JSONmanager& json = JSONmanager::GetInstance();
        if (!json.ResetReadStream(filename)) {
            AddConsoleMessage("Failed to load game settings file.");
            return;
        }

        auto settingsData = json.findValue("settings");
		// If missing data or not an object, create new settings
        if (!settingsData || !settingsData->get().IsObject()) {
            AddConsoleMessage("Invalid settings format. Creating new settings.");
            return;
        }

        // store temp old values to restore keys
        auto oldValues = values;
        values.clear();

        // Load settings
        if (settingsData && settingsData->get().IsObject()) {
            const auto& obj = settingsData->get();
            for (auto it = obj.MemberBegin(); it != obj.MemberEnd(); ++it) {
                const std::string id = it->name.GetString();

                if (it->value.IsFloat()) {
                    values[id] = it->value.GetFloat();
                }
                else if (it->value.IsBool()) {
                    values[id] = it->value.GetBool();
                }
                else if (it->value.IsInt()) {
                    values[id] = it->value.GetInt();
                }
                else {
                    // If type unknown, restore old values
                    values[id] = oldValues[id];
                }
            }
        }

        // Check all known settings and assign defaults if missing
        for (const auto& category : categories) {
            for (const auto& setting : category.settings) {
                if (values.find(setting.id) == values.end()) {
                    // Use default if available
                    switch (setting.type) {
                    case SettingType::SLIDER:
                        if (std::holds_alternative<float>(setting.defaultValue)) {
                            values[setting.id] = std::get<float>(setting.defaultValue);
                        }
                        break;
                    case SettingType::CHECKBOX:
                        if (std::holds_alternative<bool>(setting.defaultValue)) {
                            values[setting.id] = std::get<bool>(setting.defaultValue);
                        }
                        break;
                    case SettingType::DROPDOWN:
                        if (std::holds_alternative<int>(setting.defaultValue)) {
                            values[setting.id] = std::get<int>(setting.defaultValue);
                        }
                        break;
                    }
                }
            }
        }

        // Save updated settings
        SaveSettings(filename);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Failed to load settings: " << e.what() << std::endl;
        AddConsoleMessage("Failed to load game settings.");
    }
}

void SettingsManager::SaveSettings(const std::string& filename) const {
    try {
        std::filesystem::path filePath(filename);
        if (filePath.has_parent_path()) {
            std::filesystem::create_directories(filePath.parent_path());
        }

        std::ofstream createFile(filename);
        if (!createFile.is_open()) {
            std::cerr << "Failed to create settings file: " << filename << std::endl;
            return;
        }

        // Start with empty JSON
        createFile << "{}" << std::endl;
        createFile.close();

        JSONmanager& json = JSONmanager::GetInstance();
        if (!json.OpenWriteStream(filename)) {
            std::cerr << "Failed to open write stream for: " << filename << std::endl;
        }

        // Write settings
        json.StartNewObject("settings");
        for (const auto& [id, value] : values) {
            if (std::holds_alternative<float>(value)) {
                json.AddKeyValuePair(id, std::get<float>(value));
            }
            else if (std::holds_alternative<bool>(value)) {
                json.AddKeyValuePair(id, std::get<bool>(value));
            }
            else if (std::holds_alternative<int>(value)) {
                json.AddKeyValuePair(id, std::get<int>(value));
            }
        }
        json.EndObject();
        json.EndWriteStream();

        std::cout << "Settings saved to " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save settings: " << e.what() << std::endl;
    }
}

void SettingsManager::RenderSettingsMenu() {

    int windowWidth, windowHeight;
    if (SettingsManager::GetInstance().window) {
#ifdef _DEBUG
        glfwGetFramebufferSize(SettingsManager::GetInstance().window, &windowWidth, &windowHeight);
        windowWidth = static_cast <int>(windowWidth * 0.666f);
        windowHeight -= 300;
        ImGui::SetNextWindowPos(ImVec2(0, 50));
#else
        glfwGetWindowSize(SettingsManager::GetInstance().window, &windowWidth, &windowHeight);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
#endif
    }
    else {
        windowWidth = 800;
        windowHeight = 600;
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        AddConsoleMessage("Window is not set in SettingsManager.");
    }

    ImGui::SetNextWindowSize(ImVec2((float)windowWidth, (float)windowHeight));
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    static bool firstRender = true;
    if (firstRender) {
        // Focus this window the first time it's rendered
        ImGui::SetNextWindowFocus();
        firstRender = false;
    }

	bool needFocusRightPanel = false;
    // Left panel
	ImGui::BeginGroup();

    ImVec2 leftPanelSize = ImGui::GetContentRegionAvail();
    float buttonHeight = ImGui::GetFrameHeightWithSpacing();
    float spacing = ImGui::GetStyle().ItemSpacing.y;
    ImGui::BeginChild("categories", ImVec2(200, leftPanelSize.y - buttonHeight - spacing), true);
    for (const auto& category : categories) {
        // Selectable categories to switch the right side panel
        if (ImGui::Selectable(category.name.c_str(), currentCategory == category.id)) {
            currentCategory = category.id;
            needFocusRightPanel = true;
        }
    }
    ImGui::EndChild();

    ImGui::BeginChild("categories_buttons", ImVec2(200, 0), false);
    if (ImGui::Button("Save", ImVec2(100, 0))) {
        SaveSettings("settings_game.json");
        AddConsoleMessage("Game settings saved successfully.");
    }
	ImGui::SameLine();
    if (ImGui::Button("Back", ImVec2(100, 0))) {
        firstRender = true;
		MenuManager::GetInstance().HandleBackAction();
    }
    ImGui::EndChild();
    ImGui::EndGroup();

    ImGui::SameLine();

    // Right panel
    ImGui::BeginChild("settings", ImVec2(0, 0), true);
    for (const auto& category : categories) {
        if (category.id == currentCategory) {
            for (const auto& setting : category.settings) {
                ImGui::PushID(setting.id.c_str());
                // Render each setting control
                RenderSettingItem(setting);
                ImGui::PopID();
            }
        }
    }
    ImGui::EndChild();
    if (needFocusRightPanel) {
        ImGui::SetKeyboardFocusHere(-1);
		AddConsoleMessage("Focus set to right panel.");
		isRightPanelFocused = true;
        needFocusRightPanel = false;
    }
	// std::cout << isRightPanelFocused << std::endl;
    ImGui::End();
}

void SettingsManager::RenderSettingItem(const SettingConfig& config) {
    // Get the current value from settings
    auto currentValue = GetValue(config.id);
	float labelWidth = 400.0f;
    switch (config.type) {
    case SettingType::SLIDER: {
        float value = std::get<float>(currentValue);
        // Slider to adjust a float value within a range
        ImGui::PushItemWidth(labelWidth);
        if (ImGui::SliderFloat(config.label.c_str(), &value, config.minValue, config.maxValue, "%.1f")) {
            SetValue(config.id, value);
        }
		ImGui::PopItemWidth();
        break;
    }
    case SettingType::CHECKBOX: {
        bool value = std::get<bool>(currentValue);
        // Checkbox to toggle a boolean value
        if (ImGui::Checkbox(config.label.c_str(), &value)) {
            SetValue(config.id, value);
        }
        break;
    }
    case SettingType::DROPDOWN: {
        int value = std::get<int>(currentValue);
        // Dropdown (Combo) for selecting an option from a list
        ImGui::PushItemWidth(labelWidth);
        if (ImGui::Combo(config.label.c_str(), &value,
            [](void* data, int idx, const char** out_text) {
                // Cast the user data back to a reference of the vector<string> that holds the options.
                auto& options = *(std::vector<std::string>*)data;
				// make sure the index is valid
                if (idx < 0 || idx >= (int)options.size()) return false;
                // assign string of the selected option to out_text
                *out_text = options[idx].c_str();
                return true;
            },
            // config options passed to the callback function
            (void*)&config.options,
			// number of options
            (int)config.options.size())) {
            SetValue(config.id, value);
        }
		ImGui::PopItemWidth();
        break;
    }
    }
}

void SettingsManager::RegisterCallback(const std::string& settingId, SettingCallback callback) {
    // Register a callback to be triggered whenever the setting with 'settingId' changes
    callbacks[settingId] = callback;

    // Immediately invoke the callback with the current value if exists
    auto it = values.find(settingId);
    if (it != values.end()) {
        callback(it->second);
    }
}
