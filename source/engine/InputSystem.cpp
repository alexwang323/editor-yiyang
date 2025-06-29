#include "ginkgopch.h"
#include <GL/glew.h> 
#include "InputSystem.h"
#include "menuManager.h" 
#include "myphysicsdirector.h"
#include "consoleMessage.h"
#include <GLFW/glfw3.h>
#include "imgui.h"

void InputSystem::Initialize(GLFWwindow* window, InputMode mode) {
    this->window = window;
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
    mouseDeltaX = 0.0;
    mouseDeltaY = 0.0;
    DetectGamepad();

    InitializeKeyNameMap();

    // default key mappings for player 1 and player 2
    keyMappingKeyboard1[Action::MOVE_FORWARD] = GLFW_KEY_W;
    keyMappingKeyboard1[Action::MOVE_BACKWARD] = GLFW_KEY_S;
    keyMappingKeyboard1[Action::MOVE_LEFT] = GLFW_KEY_A;
    keyMappingKeyboard1[Action::MOVE_RIGHT] = GLFW_KEY_D;
    keyMappingKeyboard1[Action::JUMP] = GLFW_KEY_SPACE;

    keyMappingKeyboard2[Action::MOVE_FORWARD] = GLFW_KEY_UP;
    keyMappingKeyboard2[Action::MOVE_BACKWARD] = GLFW_KEY_DOWN;
    keyMappingKeyboard2[Action::MOVE_LEFT] = GLFW_KEY_LEFT;
    keyMappingKeyboard2[Action::MOVE_RIGHT] = GLFW_KEY_RIGHT;
    keyMappingKeyboard2[Action::JUMP] = GLFW_KEY_RIGHT_CONTROL;

    keyMappingGamepad1[Action::MOVE_FORWARD] = GLFW_GAMEPAD_BUTTON_DPAD_UP;
    keyMappingGamepad1[Action::MOVE_BACKWARD] = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
    keyMappingGamepad1[Action::MOVE_LEFT] = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
    keyMappingGamepad1[Action::MOVE_RIGHT] = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
    keyMappingGamepad1[Action::JUMP] = GLFW_GAMEPAD_BUTTON_A;

    keyMappingGamepad2[Action::MOVE_FORWARD] = GLFW_GAMEPAD_BUTTON_DPAD_UP;
    keyMappingGamepad2[Action::MOVE_BACKWARD] = GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
    keyMappingGamepad2[Action::MOVE_LEFT] = GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
    keyMappingGamepad2[Action::MOVE_RIGHT] = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
    keyMappingGamepad2[Action::JUMP] = GLFW_GAMEPAD_BUTTON_A;
}

InputSystem& InputSystem::GetInstance() {
    static InputSystem instance;
    return instance;
}

InputSystem::InputSystem()
    : camera(nullptr), firstMouse(true), lastX(400.0f),
    lastY(300.0f), isSceneViewFocused(false), gamepadId(-1),
    isYAxisInverted(false), isGamepadYAxisInverted(false) {    
}

InputSystem::~InputSystem() {
}

void InputSystem::InitializeKeyNameMap() {
    // common keys used in game
    keyNameMap["W"] = GLFW_KEY_W;
    keyNameMap["A"] = GLFW_KEY_A;
    keyNameMap["S"] = GLFW_KEY_S;
    keyNameMap["D"] = GLFW_KEY_D;
    keyNameMap["E"] = GLFW_KEY_E;
    keyNameMap["R"] = GLFW_KEY_R;
    keyNameMap["Q"] = GLFW_KEY_Q;
    keyNameMap["F"] = GLFW_KEY_F;
    
	// same for lowercase
    keyNameMap["w"] = GLFW_KEY_W;
    keyNameMap["a"] = GLFW_KEY_A;
    keyNameMap["s"] = GLFW_KEY_S;
    keyNameMap["d"] = GLFW_KEY_D;
    keyNameMap["e"] = GLFW_KEY_E;
    keyNameMap["r"] = GLFW_KEY_R;
    keyNameMap["q"] = GLFW_KEY_Q;
    keyNameMap["f"] = GLFW_KEY_F;

	// other keys
    keyNameMap["SPACE"] = GLFW_KEY_SPACE;
    keyNameMap["ENTER"] = GLFW_KEY_ENTER;
    keyNameMap["ESCAPE"] = GLFW_KEY_ESCAPE;
    keyNameMap["SHIFT"] = GLFW_KEY_LEFT_SHIFT;
    keyNameMap["CTRL"] = GLFW_KEY_LEFT_CONTROL;
    keyNameMap["ALT"] = GLFW_KEY_LEFT_ALT;
    keyNameMap["UP"] = GLFW_KEY_UP;
    keyNameMap["DOWN"] = GLFW_KEY_DOWN;
    keyNameMap["LEFT"] = GLFW_KEY_LEFT;
    keyNameMap["RIGHT"] = GLFW_KEY_RIGHT;
}

int InputSystem::GetKeyCode(const std::string& keyName) const {
    auto it = keyNameMap.find(keyName);
    if (it != keyNameMap.end()) {
        return it->second;
    }

    if (keyName.length() == 1) {
        char c = keyName[0];

        if (c >= 'a' && c <= 'z') {
            return GLFW_KEY_A + (c - 'a');
        }
        if (c >= 'A' && c <= 'Z') {
            return GLFW_KEY_A + (c - 'A');
        }
        if (c >= '0' && c <= '9') {
            return GLFW_KEY_0 + (c - '0');
        }
    }

    return -1;
}

void InputSystem::RegisterCallbacks() {
    auto& settings = SettingsManager::GetInstance();

    settings.RegisterCallback("mouse_sensitivity",
        [this](const SettingValue& value) {
            SetSensitivity(std::get<float>(value));
        });

    settings.RegisterCallback("invert_mouse_y",
        [this](const SettingValue& value) {
            SetYAxisInverted(std::get<bool>(value));
        });

    settings.RegisterCallback("gamepad_sensitivity",
        [this](const SettingValue& value) {
            SetGamepadSensitivity(std::get<float>(value));
        });

    settings.RegisterCallback("invert_gamepad_y",
        [this](const SettingValue& value) {
            SetGamepadYAxisInverted(std::get<bool>(value));
        });
}

void InputSystem::SetSceneViewFocused(bool focused) {
    isSceneViewFocused = focused;
}

void InputSystem::ResetFirstMouse() {
    firstMouse = true;
}

void InputSystem::ProcessInput(float deltaTime) {
	// update key states for both modes
    UpdateKeyStates();

    if (mode == InputMode::GAME) {
		// gamepad disconnect handling
        if (player1Method == InputMethod::GAMEPAD && gamepadIdPlayer1 == -1) {
            AssignPlayerInputMethod(1, InputMethod::KEYBOARD);
#ifdef _DEBUG
            AddConsoleMessage("Player 1 gamepad disconnected, switching to keyboard.");
#else
            std::cout << "Player 1 gamepad disconnected, switching to keyboard." << std::endl;
#endif
        }

        if (player2Method == InputMethod::GAMEPAD && gamepadIdPlayer2 == -1) {
            AssignPlayerInputMethod(2, InputMethod::KEYBOARD);
#ifdef _DEBUG
            AddConsoleMessage("Player 2 gamepad disconnected, switching to keyboard.");
#else
            std::cout << "Player 2 gamepad disconnected, switching to keyboard." << std::endl;
#endif
        }
    }

    DetectGamepad();
    ProcessMenuInput();
    
    if (mode == InputMode::EDITOR) {
        ProcessGamepadInput(deltaTime);
        if (!camera) {
            return;
        }
        ProcessEditorInput(deltaTime);
    }
    else { // InputMode::GAME
        if (!camera) {
            return;
        }
        ProcessGameInput(deltaTime);
    }
}

void InputSystem::ProcessEditorInput(float deltaTime) {
#ifdef _DEBUG
    if (isSceneViewFocused) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            if (firstMouse) {
                lastX = static_cast<float>(xpos);
                lastY = static_cast<float>(ypos);
                firstMouse = false;
            }

            float xOffset = static_cast<float>(xpos) - lastX;
            float yOffset = lastY - static_cast<float>(ypos);

            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);

            xOffset *= sensitivity;
            yOffset *= (isYAxisInverted ? -sensitivity : sensitivity);

            camera->RotateCamera(xOffset, yOffset);
        }
        else {
            firstMouse = true;
        }
    }
#else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported()) {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        float xOffset = static_cast<float>(xpos - lastX);
        float yOffset = static_cast<float>(lastY - ypos);

        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        xOffset *= sensitivity;
        yOffset *= (isYAxisInverted ? -sensitivity : sensitivity);

        camera->RotateCamera(xOffset, yOffset);
    }
#endif

    MenuState currentState = MenuManager::GetInstance().GetCurrentMenuState();
    if (currentState != MenuState::NONE)
        return;

    float cameraSpeed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 5.0f;
    }
    glm::vec3 front = camera->GetFront();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, front));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->TranslateCamera(cameraSpeed * front);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->TranslateCamera(-cameraSpeed * front);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->TranslateCamera(-cameraSpeed * right);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->TranslateCamera(cameraSpeed * right);
    }

    if (Myphysicsdirector::player) {
        glm::vec3 tmpdir(0, 0, 0);
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            tmpdir += 100.0f * (cameraSpeed * front);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            tmpdir += 100.0f * (-cameraSpeed * front);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            tmpdir += 100.0f * (-cameraSpeed * right);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            tmpdir += 100.0f * (cameraSpeed * right);
        }
        Myphysicsdirector::player->set_velocity_on_axis(tmpdir, glm::vec3(0, 1, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        camera->TranslateCamera(cameraSpeed * up);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera->TranslateCamera(-cameraSpeed * up);
    }
}

// Game mode input
void InputSystem::ProcessGameInput(float deltaTime) {
    // DisplayKeyMappingDebug();
    // DisplayInputAssignmentUI();
    // TODO: Implement game-mode input logic using key mapping and command pattern.
}

void InputSystem::ProcessMenuInput() {
    static bool wasEscapePressed = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!wasEscapePressed) {
            MenuManager& menuManager = MenuManager::GetInstance();
            if (menuManager.GetCurrentMenuState() == MenuState::NONE) {
                menuManager.SetCurrentMenuState(MenuState::IN_GAME_MENU);
            }
            else if (menuManager.GetCurrentMenuState() == MenuState::IN_GAME_MENU) {
                menuManager.SetCurrentMenuState(MenuState::NONE);
            }
            else if (menuManager.GetCurrentMenuState() == MenuState::SETTINGS_MENU) {
                menuManager.SetCurrentMenuState(menuManager.GetPreviousMenuState());
            }
            wasEscapePressed = true;
        }
    }
    else {
        wasEscapePressed = false;
    }
}

void InputSystem::DetectGamepad() {
    // detect gamepad connection status
    static bool gamepadConnected = false;

    int newGamepadId1 = -1;
    int newGamepadId2 = -1;
    int connectedCount = 0;

    // iterate through all possible gamepad IDs
    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (glfwJoystickPresent(jid) && glfwJoystickIsGamepad(jid)) {
            connectedCount++;
            if (newGamepadId1 == -1) {
                newGamepadId1 = jid;
            }
            else if (newGamepadId2 == -1) {
                newGamepadId2 = jid;
            }
        }
    }

    bool currentlyConnected = (newGamepadId1 != -1);
    if (currentlyConnected != gamepadConnected) {
        gamepadConnected = currentlyConnected;
#ifdef _DEBUG
        if (gamepadConnected) {
            AddConsoleMessage("Gamepad connected. Connected count: " + std::to_string(connectedCount));
        }
        else {
            AddConsoleMessage("Gamepad disconnected. Connected count: " + std::to_string(connectedCount));
        }
#else
        if (gamepadConnected) {
            std::cout << "Gamepad connected. Connected count: " << connectedCount << std::endl;
        }
        else {
            std::cout << "Gamepad disconnected. Connected count: " << connectedCount << std::endl;
        }
#endif
        }

	// update gamepad ID based on current mode
    if (mode == InputMode::EDITOR) {
		// in editor mode, only use the first detected gamepad
        gamepadIdPlayer1 = newGamepadId1;
        gamepadIdPlayer2 = -1;
        gamepadId = newGamepadId1;
    }
    else { // game mode
		// if only one gamepad is connected, assign it to both players
        if (newGamepadId1 != -1 && newGamepadId2 == -1) {
            newGamepadId2 = newGamepadId1;
        }
        gamepadIdPlayer1 = newGamepadId1;
        gamepadIdPlayer2 = newGamepadId2;
    }
}

float InputSystem::ApplyDeadzone(float value, float deadzone) {
    return (std::abs(value) < deadzone) ? 0.0f : value;
}

void InputSystem::ProcessGamepadInput(float deltaTime) {
	// old code just for editor mode
    if (gamepadId == -1) {
        DetectGamepad();
        return;
    }

    GLFWgamepadstate state;
    if (glfwGetGamepadState(gamepadId, &state)) {
        float rx = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
        float ry = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

        float lx = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
        float ly = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

        const float deadzone = 0.1f;
        const float rotationSpeed = 100.0f;

        rx = ApplyDeadzone(rx, deadzone);
        ry = ApplyDeadzone(ry, deadzone);
        if (isGamepadYAxisInverted) {
            ry = -ry;
        }

        if (rx != 0.0f || ry != 0.0f) {
            float xRotation = rx * gamePadSensitivity * rotationSpeed * deltaTime;
            float yRotation = -ry * gamePadSensitivity * rotationSpeed * deltaTime;
            camera->RotateCamera(xRotation, yRotation);
        }

        lx = ApplyDeadzone(lx, deadzone);
        ly = ApplyDeadzone(ly, deadzone);

        glm::vec3 front = camera->GetFront();
        glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

        float moveSpeed = 5.0f;
        if (ly != 0.0f) {
            camera->TranslateCamera(moveSpeed * (-ly) * front * deltaTime);
        }
        if (lx != 0.0f) {
            camera->TranslateCamera(moveSpeed * lx * right * deltaTime);
        }

        static bool wasBPressed = false;
        if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
            if (!wasBPressed) {
                MenuManager& menuManager = MenuManager::GetInstance();
                MenuState currentState = menuManager.GetCurrentMenuState();
                if (currentState == MenuState::SETTINGS_MENU) {
                    if (SettingsManager::GetInstance().IsRightPanelFocused()) {
                        ImGui::SetWindowFocus("categories");
                        SettingsManager::GetInstance().SetRightPanelFocused(false);
                    }
                    else {
                        menuManager.SetCurrentMenuState(menuManager.GetPreviousMenuState());
                    }
                }
                else if (currentState == MenuState::IN_GAME_MENU) {
                    menuManager.SetCurrentMenuState(MenuState::NONE);
                }
                wasBPressed = true;
            }
        }
        else {
            wasBPressed = false;
        }

        static bool wasStartPressed = false;
        if (state.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS) {
            if (!wasStartPressed) {
                MenuState currentState = MenuManager::GetInstance().GetCurrentMenuState();
                if (currentState == MenuState::NONE) {
                    MenuManager::GetInstance().SetCurrentMenuState(MenuState::IN_GAME_MENU);
                }
                else if (currentState == MenuState::IN_GAME_MENU) {
                    MenuManager::GetInstance().SetCurrentMenuState(MenuState::NONE);
                }
                else if (currentState == MenuState::SETTINGS_MENU) {
                    MenuManager::GetInstance().SetCurrentMenuState(MenuManager::GetInstance().GetPreviousMenuState());
                }
                wasStartPressed = true;
            }
        }
        else {
            wasStartPressed = false;
        }

        ImGuiIO& io = ImGui::GetIO();
        static bool wasUpPressed = false;
        static bool wasDownPressed = false;
        static bool wasLeftPressed = false;
        static bool wasRightPressed = false;

        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS) {
            if (!wasUpPressed) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadUp, true);
                wasUpPressed = true;
            }
        }
        else if (wasUpPressed) {
            io.AddKeyEvent(ImGuiKey_GamepadDpadUp, false);
            wasUpPressed = false;
        }

        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS) {
            if (!wasDownPressed) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadDown, true);
                wasDownPressed = true;
            }
        }
        else if (wasDownPressed) {
            io.AddKeyEvent(ImGuiKey_GamepadDpadDown, false);
            wasDownPressed = false;
        }

        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS) {
            if (!wasLeftPressed) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, true);
                wasLeftPressed = true;
            }
        }
        else if (wasLeftPressed) {
            io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, false);
            wasLeftPressed = false;
        }

        if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS) {
            if (!wasRightPressed) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadRight, true);
                wasRightPressed = true;
            }
        }
        else if (wasRightPressed) {
            io.AddKeyEvent(ImGuiKey_GamepadDpadRight, false);
            wasRightPressed = false;
        }

        static bool wasAPressed = false;
        if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
            if (!wasAPressed) {
                io.AddKeyEvent(ImGuiKey_GamepadFaceDown, true);
                wasAPressed = true;
            }
        }
        else if (wasAPressed) {
            io.AddKeyEvent(ImGuiKey_GamepadFaceDown, false);
            wasAPressed = false;
        }
    }
    else {
#ifdef _DEBUG
        AddConsoleMessage("Gamepad disconnected or state unavailable: " + std::to_string(gamepadId));
#else
        std::cout << "Gamepad disconnected or state unavailable: " << gamepadId << std::endl;
#endif
        gamepadId = -1;
    }
}

/*
bool InputSystem::IsKeyDown(int key) const {
    return currentKeyStatesPlayer1.count(key) ? currentKeyStatesPlayer1.at(key) : false;
}

bool InputSystem::IsKeyJustPressed(int key) const {
    bool curr = currentKeyStatesPlayer1.count(key) ? currentKeyStatesPlayer1.at(key) : false;
    bool prev = previousKeyStatesPlayer1.count(key) ? previousKeyStatesPlayer1.at(key) : false;
    return curr && !prev;
}

bool InputSystem::IsKeyJustReleased(int key) const {
    bool curr = currentKeyStatesPlayer1.count(key) ? currentKeyStatesPlayer1.at(key) : false;
    bool prev = previousKeyStatesPlayer1.count(key) ? previousKeyStatesPlayer1.at(key) : false;
    return (!curr && prev);
}
*/

bool InputSystem::IsKeyDown(const std::string& keyName) const {
    int keyCode = GetKeyCode(keyName);
    if (keyCode != -1) {
        return (currentKeyStatesPlayer1.count(keyCode) ? currentKeyStatesPlayer1.at(keyCode) : false);
    }
    return false;
}

bool InputSystem::IsKeyJustPressed(const std::string& keyName) const {
    int keyCode = GetKeyCode(keyName);
    if (keyCode != -1) {
        bool curr = currentKeyStatesPlayer1.count(keyCode) ? currentKeyStatesPlayer1.at(keyCode) : false;
        bool prev = previousKeyStatesPlayer1.count(keyCode) ? previousKeyStatesPlayer1.at(keyCode) : false;
        return curr && !prev;
    }
    return false;
}

bool InputSystem::IsKeyJustReleased(const std::string& keyName) const {
    int keyCode = GetKeyCode(keyName);
    if (keyCode != -1) {
        bool curr = currentKeyStatesPlayer1.count(keyCode) ? currentKeyStatesPlayer1.at(keyCode) : false;
        bool prev = previousKeyStatesPlayer1.count(keyCode) ? previousKeyStatesPlayer1.at(keyCode) : false;
        return (!curr && prev);
    }
    return false;
}

bool InputSystem::IsPlayerActionPressed(int player, Action action) const {
    int key = -1;
    if (player == 1) {
        if (player1Method == InputMethod::GAMEPAD) {
            if (keyMappingGamepad1.count(action) == 0) {
                return false;
            }
            key = keyMappingGamepad1.at(action);
        }
        else {  // KEYBOARD
            if (keyMappingKeyboard1.count(action) == 0) {
                return false;
            }
            key = keyMappingKeyboard1.at(action);
        }
        return (currentKeyStatesPlayer1.count(key) ? currentKeyStatesPlayer1.at(key) : false);
    }
    else if (player == 2) {
        if (player2Method == InputMethod::GAMEPAD) {
            if (keyMappingGamepad2.count(action) == 0) {
                return false;
            }
            key = keyMappingGamepad2.at(action);
        }
        else {  // KEYBOARD
            if (player1Method != InputMethod::KEYBOARD) {
                if (keyMappingKeyboard1.count(action) == 0) {
                    return false;
                }
                key = keyMappingKeyboard1.at(action);
            }
            else {
                if (keyMappingKeyboard2.count(action) == 0) {
                    return false;
                }

                key = keyMappingKeyboard2.at(action);
            }
        }
        return (currentKeyStatesPlayer2.count(key) ? currentKeyStatesPlayer2.at(key) : false);
    }
    return false;
}

bool InputSystem::IsPlayerActionJustPressed(int player, Action action) const {
    int key = -1;
    if (player == 1) {
        if (player1Method == InputMethod::GAMEPAD) {
            if (keyMappingGamepad1.count(action) == 0) {
                return false;
            }
            key = keyMappingGamepad1.at(action);
        }
        else {
            if (keyMappingKeyboard1.count(action) == 0) {
                return false;
            }
            key = keyMappingKeyboard1.at(action);
        }
        bool curr = (currentKeyStatesPlayer1.count(key) ? currentKeyStatesPlayer1.at(key) : false);
        bool prev = (previousKeyStatesPlayer1.count(key) ? previousKeyStatesPlayer1.at(key) : false);
        return (curr && !prev);
    }
    else if (player == 2) {
        if (player2Method == InputMethod::GAMEPAD) {
            if (keyMappingGamepad2.count(action) == 0) {
                return false;
            }
            key = keyMappingGamepad2.at(action);
        }
        else {
            if (player1Method != InputMethod::KEYBOARD) {
                if (keyMappingKeyboard1.count(action) == 0) {
                    return false;
                }
                key = keyMappingKeyboard1.at(action);
            }
            else {
                if (keyMappingKeyboard2.count(action) == 0) {
                    return false;
                }
                key = keyMappingKeyboard2.at(action);
            }
        }
        bool curr = (currentKeyStatesPlayer2.count(key) ? currentKeyStatesPlayer2.at(key) : false);
        bool prev = (previousKeyStatesPlayer2.count(key) ? previousKeyStatesPlayer2.at(key) : false);
        return (curr && !prev);
    }
    return false;
}

bool InputSystem::IsPlayerActionJustReleased(int player, Action action) const {
    int key = -1;
    if (player == 1) {
        if (player1Method == InputMethod::GAMEPAD) {
            if (keyMappingGamepad1.count(action) == 0) {
                return false;
            }
            key = keyMappingGamepad1.at(action);
        }
        else {
            if (keyMappingKeyboard1.count(action) == 0) {
                return false;
            }
            key = keyMappingKeyboard1.at(action);
        }
        bool curr = (currentKeyStatesPlayer1.count(key) ? currentKeyStatesPlayer1.at(key) : false);
        bool prev = (previousKeyStatesPlayer1.count(key) ? previousKeyStatesPlayer1.at(key) : false);
        return (!curr && prev);
    }
    else if (player == 2) {
        if (player2Method == InputMethod::GAMEPAD) {
            if (keyMappingGamepad2.count(action) == 0) {
                return false;
            }
            key = keyMappingGamepad2.at(action);
        }
        else {
            if (keyMappingKeyboard2.count(action) == 0) {
                return false;
            }
            key = keyMappingKeyboard2.at(action);
        }
        bool curr = (currentKeyStatesPlayer2.count(key) ? currentKeyStatesPlayer2.at(key) : false);
        bool prev = (previousKeyStatesPlayer2.count(key) ? previousKeyStatesPlayer2.at(key) : false);
        return (!curr && prev);
    }
    return false;
}

void InputSystem::DisplayKeyMappingDebug() {

    ImGui::Begin("Key Mapping Debug Info");

	// current actions
    const char* actionNames[] = { 
        "MOVE_FORWARD", 
        "MOVE_BACKWARD", 
        "MOVE_LEFT", 
        "MOVE_RIGHT", 
        "JUMP" 
    };
    Action actions[] = { 
        Action::MOVE_FORWARD, 
        Action::MOVE_BACKWARD, 
        Action::MOVE_LEFT, 
        Action::MOVE_RIGHT, 
        Action::JUMP 
    };

	int actionCount = sizeof(actions) / sizeof(actions[0]);

	// player 1 actions
    ImGui::Text("Player 1 Actions:");
    for (int i = 0; i < actionCount; i++) {
        const char* name = actionNames[i];
        bool pressed = IsPlayerActionPressed(1, actions[i]);
        ImGui::Text("%s:", name);
        ImGui::SameLine();
        if (pressed) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pressed");
        }
        else {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Released");
        }
    }

    ImGui::Separator();

	// player 2 actions
    ImGui::Text("Player 2 Actions:");
    for (int i = 0; i < actionCount; i++) {
        const char* name = actionNames[i];
        bool pressed = IsPlayerActionPressed(2, actions[i]);
        ImGui::Text("%s:", name);
        ImGui::SameLine();
        if (pressed) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pressed");
        }
        else {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Released");
        }
    }

    ImGui::Separator();

	// show mouse state (idk if we need mouse manipulation in the game)
    ImGui::Text("Mouse State:");
    bool leftClicked = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
    bool rightClicked = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
    ImGui::Text("LEFT_CLICK:");
    ImGui::SameLine();
    if (leftClicked) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pressed");
    }
    else {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Released");
    }
    ImGui::Text("RIGHT_CLICK:");
    ImGui::SameLine();
    if (rightClicked) {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Pressed");
    }
    else {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Released");
    }

    ImGui::End();
}

void InputSystem::AssignPlayerInputMethod(int player, InputMethod method, int gamepadId) {
    if (player == 1) {
        player1Method = method;
        if (method == InputMethod::GAMEPAD) {
            gamepadIdPlayer1 = gamepadId;
			//AddConsoleMessage("Player 1 assigned to gamepad: " + std::to_string(gamepadId));
        }
        else {
            gamepadIdPlayer1 = -1;
			//AddConsoleMessage("Player 1 assigned to keyboard");
        }
    }
    else if (player == 2) {
        player2Method = method;
        if (method == InputMethod::GAMEPAD) {
            gamepadIdPlayer2 = gamepadId;
        }
    }
}

void InputSystem::DisplayInputAssignmentUI() {
    if (mode == InputMode::EDITOR) {
        ImGui::Begin("Input Assignment");
        ImGui::Text("Mode: Editor");
        ImGui::Separator();

		// just show the detected input devices
        std::vector<std::string> devices;
        devices.push_back("Keyboard");
        if (gamepadIdPlayer1 != -1) {
            devices.push_back("Gamepad 1");
        }
        if (gamepadIdPlayer2 != -1 && gamepadIdPlayer2 != gamepadIdPlayer1) {
            devices.push_back("Gamepad 2");
        }

        ImGui::Text("Detected Input Devices:");
        for (const auto& dev : devices) {
            ImGui::Text("%s", dev.c_str());
        }
        ImGui::End();
    }
    else {
        // inner struct for input device
        struct InputDevice {
            std::string name;
            InputMethod type;
            int deviceId; // if gamepad, store the device id, otherwise -1 (keyboard)
        };

        // set up the list of default devices (only keyboard)
        std::vector<std::string> allDeviceNames;
        std::vector<InputDevice> devices;

        devices.push_back({ "Keyboard", InputMethod::KEYBOARD, -1 });

        // if gamepad is connected, add it to the list
        if (gamepadIdPlayer1 != -1) {
            devices.push_back({ "Gamepad 1", InputMethod::GAMEPAD, gamepadIdPlayer1 });
        }
        if (gamepadIdPlayer2 != -1 && gamepadIdPlayer2 != gamepadIdPlayer1) {
            devices.push_back({ "Gamepad 2", InputMethod::GAMEPAD, gamepadIdPlayer2 });
        }

        // list of all devices
        for (const auto& dev : devices) {
            allDeviceNames.push_back(dev.name);
        }

        // set list of available devices for each player
        std::vector<InputDevice> listP1 = devices;
        std::vector<InputDevice> listP2 = devices;

        // selected device for each player, default is keyboard
        static std::string player1Assigned = "Keyboard";
        static std::string player2Assigned = "Keyboard";

        // filter logic: only filter gamepads
        // for player 1: if player 2 selected device is not keyboard (a gamepad), remove that device from player 1's list
        if (player2Assigned != "Keyboard" && !player2Assigned.empty()) {
            for (auto it = listP1.begin(); it != listP1.end(); ) {
                if (it->name == player2Assigned)
                    it = listP1.erase(it);
                else
                    ++it;
            }
        }
        // same logic for player 2
        if (player1Assigned != "Keyboard" && !player1Assigned.empty()) {
            for (auto it = listP2.begin(); it != listP2.end(); ) {
                if (it->name == player1Assigned)
                    it = listP2.erase(it);
                else
                    ++it;
            }
        }

        // create vectors for combobox
        std::vector<const char*> deviceNamesP1;
        for (const auto& dev : listP1) {
            deviceNamesP1.push_back(dev.name.c_str());
        }
        std::vector<const char*> deviceNamesP2;
        for (const auto& dev : listP2) {
            deviceNamesP2.push_back(dev.name.c_str());
        }

        // find the selected index for each player
        int selectedIndexP1 = 0;
        for (int i = 0; i < (int)listP1.size(); i++) {
            if (listP1[i].name == player1Assigned) {
                selectedIndexP1 = i;
                break;
            }
        }

        int selectedIndexP2 = 0;
        for (int i = 0; i < (int)listP2.size(); i++) {
            if (listP2[i].name == player2Assigned) {
                selectedIndexP2 = i;
                break;
            }
        }

        ImGui::Begin("Input Assignment");
        ImGui::Text("Assign input method for each player:");

        // player 1 combobox
        if (ImGui::Combo("Player 1", &selectedIndexP1, deviceNamesP1.data(), (int)deviceNamesP1.size())) {
            player1Assigned = listP1[selectedIndexP1].name;
            if (player1Assigned == "Keyboard") {
                AssignPlayerInputMethod(1, InputMethod::KEYBOARD);
            }
            else if (player1Assigned.find("Gamepad") != std::string::npos) {
                int id = listP1[selectedIndexP1].deviceId;
                AssignPlayerInputMethod(1, InputMethod::GAMEPAD, id);
            }
        }

        // player 2 combobox
        if (ImGui::Combo("Player 2", &selectedIndexP2, deviceNamesP2.data(), (int)deviceNamesP2.size())) {
            player2Assigned = listP2[selectedIndexP2].name;
            if (player2Assigned == "Keyboard") {
                AssignPlayerInputMethod(2, InputMethod::KEYBOARD);
            }
            else if (player2Assigned.find("Gamepad") != std::string::npos) {
                int id = listP2[selectedIndexP2].deviceId;
                AssignPlayerInputMethod(2, InputMethod::GAMEPAD, id);
            }
        }

        ImGui::Separator();
        ImGui::Text("Detected Input Devices:");
        for (const auto& dev : devices) {
            ImGui::Text("%s", dev.name.c_str());
        }
        ImGui::End();
    }
}

void InputSystem::CreateInputSystemLuaBindings(std::shared_ptr<sol::state> lua) {
    lua->new_usertype<InputSystem>("InputSystem",
        "IsKeyDown", &IsKeyDown,
        "IsKeyJustPressed", &IsKeyJustPressed,
        "IsKeyJustReleased", &IsKeyJustReleased,
        "IsPlayerActionPressed", &IsPlayerActionPressed,
        "IsPlayerActionJustPressed", &IsPlayerActionJustPressed,
        "IsPlayerActionJustReleased", &IsPlayerActionJustReleased
        );
}

void InputSystem::SendInputSystemToLua(std::shared_ptr<sol::state> lua) {
    (*lua)["InputSystem"] = std::ref(*this);
}

void InputSystem::UpdateKeyStates() {
    int commonKeys[] = {
        GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D,
        GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_Q, GLFW_KEY_F,
        GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE,
        GLFW_KEY_LEFT_SHIFT, GLFW_KEY_LEFT_CONTROL, GLFW_KEY_LEFT_ALT,
        GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT
    };

	// update key states for player 1
    for (auto& kv : currentKeyStatesPlayer1) {
        previousKeyStatesPlayer1[kv.first] = kv.second;
    }

    if (player1Method == InputMethod::KEYBOARD) {
        for (int key : commonKeys) {
            currentKeyStatesPlayer1[key] = (glfwGetKey(window, key) == GLFW_PRESS);
        }

        for (const auto& mapping : keyMappingKeyboard1) {
            int key = mapping.second;
            currentKeyStatesPlayer1[key] = (glfwGetKey(window, key) == GLFW_PRESS);
        }
/*
#ifdef _DEBUG
        bool aPressed = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
        bool dPressed = (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
        currentKeyStatesPlayer1[GLFW_KEY_A] = aPressed;
        currentKeyStatesPlayer1[GLFW_KEY_D] = dPressed;
        if (aPressed || dPressed) {
            AddConsoleMessage("Key A: " + std::string(aPressed ? "pressed" : "released") +
                ", Key D: " + std::string(dPressed ? "pressed" : "released"));
        }
#endif
*/
    }
    else if (player1Method == InputMethod::GAMEPAD && gamepadIdPlayer1 != -1) {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(gamepadIdPlayer1, &state)) {
            for (const auto& mapping : keyMappingGamepad1) {
                int button = mapping.second;
                currentKeyStatesPlayer1[button] = (state.buttons[button] == GLFW_PRESS);
            }
        }
    }

	// update key states for player 2
    for (auto& kv : currentKeyStatesPlayer2) {
        previousKeyStatesPlayer2[kv.first] = kv.second;
    }

    if (player2Method == InputMethod::KEYBOARD) {
        for (int key : commonKeys) {
            currentKeyStatesPlayer2[key] = (glfwGetKey(window, key) == GLFW_PRESS);
        }

        if (player1Method != InputMethod::KEYBOARD) {
            for (const auto& mapping : keyMappingKeyboard1) {
                int key = mapping.second;
                currentKeyStatesPlayer2[key] = (glfwGetKey(window, key) == GLFW_PRESS);
            }
        }
        else {
            for (const auto& mapping : keyMappingKeyboard2) {
                int key = mapping.second;
                currentKeyStatesPlayer2[key] = (glfwGetKey(window, key) == GLFW_PRESS);
            }
        }
    }
    else if (player2Method == InputMethod::GAMEPAD && gamepadIdPlayer2 != -1) {
        GLFWgamepadstate state;
        if (glfwGetGamepadState(gamepadIdPlayer2, &state)) {
            for (const auto& mapping : keyMappingGamepad2) {
                int button = mapping.second;
                currentKeyStatesPlayer2[button] = (state.buttons[button] == GLFW_PRESS);
            }
        }
    }
}