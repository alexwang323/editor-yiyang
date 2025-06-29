#pragma once
#include <string>
#include <variant>
#include <vector>
#include <functional>

using SettingValue = std::variant<float, bool, int>;
using SettingCallback = std::function<void(const SettingValue&)>;

enum class SettingType {
    SLIDER,
    CHECKBOX,
    DROPDOWN,
    BUTTON
};

struct SettingConfig {
    std::string label;
    std::string id;
    SettingType type;
    std::variant<float, bool, int> defaultValue;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    std::vector<std::string> options;
};

struct SettingCategory {
    std::string name;
    std::string id;
    std::vector<SettingConfig> settings;
};