#pragma once

#include <tbpch.h>
#include "Tabby/Core/Input/GamepadCodes.h"
#include "Tabby/Core/Input/KeyCodes.h"
#include "Tabby/Core/Input/MouseCodes.h"
#include <Tabby/Core/Events/MouseEvent.h>
#include <Tabby/Foundation/Types.h>

struct _SDL_Joystick;
typedef struct _SDL_Joystick SDL_Joystick;

namespace Tabby {

struct GamepadInfo {
    std::string name = "";
    void* SDL_Gamepad = nullptr;
    bool isGamepad = false;
    bool ready = false;
    float deadZone = 0.1f;
    // int axisCount;
    // int buttonCount;
};

class Input {
public:
    static void Init();

    static bool IsKeyPressed(KeyCode key);
    static bool IsMouseButtonPressed(MouseCode button);
    static Vector2 GetMousePosition();
    static Vector2 GetMouseScrollDelta();
    static float GetMouseX();
    static float GetMouseY();

    // Controller Stuff
    static const GamepadInfo& GetGamepadInfo(int index);
    static void RefreshGamepads();
    static int GetNumOfGamepads();
    static float GetGamepadAxis(int index, GamepadAxis axis);
    static bool IsGamepadButtonPressed(int index, GamepadButtons button);

private:
    Input();
    GamepadInfo m_Gamepads[MAX_GAMEPADS];
    Vector2 m_MouseScrollDelta;

private:
    static Input* s_Instance;

    friend class Application;
};

}
