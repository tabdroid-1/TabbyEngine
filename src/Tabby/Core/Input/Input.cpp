#include "Input.h"
#include "SDL.h"

namespace Tabby {

Input* Input::s_Instance = nullptr;

void Input::Init()
{
    if (s_Instance == nullptr)
        s_Instance = new Input();
}

Input::Input()
{
    s_Instance = this;

    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++) {
        GamepadInfo tempCont;

        // Check if joystick is a gamepad (ex. xbox360 and ps3 controllers)
        if (SDL_IsGameController(i)) {
            tempCont.SDL_Gamepad = SDL_GameControllerOpen(i);
            tempCont.isGamepad = true;
        } else {
            tempCont.SDL_Gamepad = SDL_JoystickOpen(i);
            tempCont.isGamepad = false;
        }

        if (tempCont.SDL_Gamepad) {
            tempCont.ready = true;
            if (tempCont.isGamepad) {
                tempCont.name = SDL_GameControllerName((SDL_GameController*)tempCont.SDL_Gamepad);
            } else {
                // tempCont.axisCount = SDL_JoystickNumAxes((SDL_Joystick*)tempCont.SDL_Gamepad);
                // tempCont.buttonCount = SDL_JoystickNumButtons((SDL_Joystick*)tempCont.SDL_Gamepad);
                tempCont.name = SDL_JoystickName((SDL_Joystick*)tempCont.SDL_Gamepad);
            }
            tempCont.name += '\0';
            m_Gamepads[i] = tempCont;
        } else
            TB_CORE_WARN("Unable to open game controller [ERROR: {0}]", SDL_GetError());
    }
}

void Input::RefreshGamepads()
{
    for (int i = 0; (i < SDL_NumJoysticks()) && (i < MAX_GAMEPADS); i++) {
        GamepadInfo tempCont;

        // Check if joystick is a gamepad (ex. xbox360 and ps3 controllers)
        if (SDL_IsGameController(i)) {
            tempCont.SDL_Gamepad = SDL_GameControllerOpen(i);
            tempCont.isGamepad = true;
        } else {
            tempCont.SDL_Gamepad = SDL_JoystickOpen(i);
            tempCont.isGamepad = false;
        }

        if (tempCont.SDL_Gamepad) {
            tempCont.ready = true;
            if (tempCont.isGamepad) {
                tempCont.name = SDL_GameControllerName((SDL_GameController*)tempCont.SDL_Gamepad);
            } else {
                // tempCont.axisCount = SDL_JoystickNumAxes((SDL_Joystick*)tempCont.SDL_Gamepad);
                // tempCont.buttonCount = SDL_JoystickNumButtons((SDL_Joystick*)tempCont.SDL_Gamepad);
                tempCont.name = SDL_JoystickName((SDL_Joystick*)tempCont.SDL_Gamepad);
            }
            tempCont.name += '\0';
            s_Instance->m_Gamepads[i] = tempCont;
        } else
            TB_CORE_WARN("Unable to open game controller [ERROR: {0}]", SDL_GetError());
    }
}

const GamepadInfo& Input::GetGamepadInfo(int index)
{
    return s_Instance->m_Gamepads[index];
}
}
