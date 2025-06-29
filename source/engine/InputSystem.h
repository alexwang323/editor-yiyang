#pragma once

#include <GLFW/glfw3.h>
#include "camera.h"
#include "settingsManager.h"
#include "consoleMessage.h"
#include <sol/sol.hpp>

enum class InputMode {
    EDITOR,
    GAME
};

enum class InputMethod {
    KEYBOARD,
    GAMEPAD
};

// Define game actions for key mapping HERE
enum class Action {
    MOVE_FORWARD,
    MOVE_BACKWARD,
    MOVE_LEFT,
    MOVE_RIGHT,
    JUMP
};

class InputSystem {
public:
    void Initialize(GLFWwindow* window, InputMode mode = InputMode::EDITOR);

    static InputSystem& GetInstance();

    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

	void SetCamera(Camera* cam) { camera = cam; }

    void RegisterCallbacks();
    void ProcessInput(float deltaTime);

    void SetSceneViewFocused(bool focused);
    void ResetFirstMouse();

    void SetSensitivity(float value) { sensitivity = value * 0.1f; }
    void SetGamepadSensitivity(float value) { gamePadSensitivity = value * 2.0f; }
    void SetYAxisInverted(bool value) { isYAxisInverted = value; }
    void SetGamepadYAxisInverted(bool value) { isGamepadYAxisInverted = value; }

    // Switch input mode between editor and game
    void SetMode(InputMode newMode) { mode = newMode; }
    InputMode GetMode() const { return mode; }

    /*
	// Key state functions (global)
    bool IsKeyDown(int key) const;
    bool IsKeyJustPressed(int key) const;
    bool IsKeyJustReleased(int key) const;
    */

	// key state functions support for key names
    bool IsKeyDown(const std::string& keyName) const;
    bool IsKeyJustPressed(const std::string& keyName) const;
    bool IsKeyJustReleased(const std::string& keyName) const;

    void UpdateKeyStates();

    bool IsPlayerActionPressed(int player, Action action) const;
    bool IsPlayerActionJustPressed(int player, Action action) const;
    bool IsPlayerActionJustReleased(int player, Action action) const;

    void AssignPlayerInputMethod(int player, InputMethod method, int gamepadId = -1);

    void DisplayKeyMappingDebug();
    void DisplayInputAssignmentUI();

    static void CreateInputSystemLuaBindings(std::shared_ptr<sol::state> lua);
    void SendInputSystemToLua(std::shared_ptr<sol::state> lua);

private:
    InputSystem();
    ~InputSystem();

    GLFWwindow* window;
    Camera* camera;
    InputMode mode;

    bool firstMouse;
    float lastX, lastY;
    float sensitivity = 0.1f;
    float gamePadSensitivity = 0.5f;

    double lastMouseX, lastMouseY;
    double mouseDeltaX, mouseDeltaY;
    const double mouseEpsilon = 0.001;

    void ProcessMenuInput();
    void ProcessGamepadInput(float deltaTime);
    void DetectGamepad();
    float ApplyDeadzone(float value, float deadzone);

    void ProcessEditorInput(float deltaTime);
    void ProcessGameInput(float deltaTime); 

    bool isSceneViewFocused;
    bool isYAxisInverted;
    bool isGamepadYAxisInverted;

    int gamepadId;

	// Player input related variables
    // Player 1:
    InputMethod player1Method = InputMethod::KEYBOARD;
    int gamepadIdPlayer1 = -1;
    std::unordered_map<Action, int> keyMappingKeyboard1;
    std::unordered_map<Action, int> keyMappingGamepad1;
    std::unordered_map<int, bool> previousKeyStatesPlayer1;
    std::unordered_map<int, bool> currentKeyStatesPlayer1;
    // Player 2:
    InputMethod player2Method = InputMethod::KEYBOARD;
    int gamepadIdPlayer2 = -1;
    std::unordered_map<Action, int> keyMappingKeyboard2;
    std::unordered_map<Action, int> keyMappingGamepad2;
    std::unordered_map<int, bool> previousKeyStatesPlayer2;
    std::unordered_map<int, bool> currentKeyStatesPlayer2;

	// key name map for key state functions
    int GetKeyCode(const std::string& keyName) const;
    std::unordered_map<std::string, int> keyNameMap;
    void InitializeKeyNameMap();

};
