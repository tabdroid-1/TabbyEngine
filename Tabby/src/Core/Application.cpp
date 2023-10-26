#include <tbpch.h>

#include "Application.h"
#include "raylib.h"
#include "rlImGui/rlImGui.h"

namespace Tabby {

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& specification)
    : m_Specification(specification)
{

    TB_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    int monitor = GetCurrentMonitor();
    InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), m_Specification.Name.c_str());

    frameBuffer = LoadRenderTexture(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));

    SetConfigFlags(FLAG_VSYNC_HINT);

    int maxHeight = GetMonitorHeight(monitor) - 40;
    if (GetScreenHeight() > maxHeight)
        SetWindowSize(GetScreenWidth(), maxHeight);

#if DEBUG

    rlImGuiSetup(true);

#endif // DEBUG

    SetExitKey(0);
    SetTargetFPS(60);
}

Application::~Application()
{
    rlImGuiShutdown();
}

void Application::Run()
{

    while (IsRunning()) {

        float deltaTime = GetFrameTime();

        sceneManager.Update(deltaTime);
        sceneManager.LateUpdate(deltaTime);

        BeginTextureMode(frameBuffer);
        // textures and stuff goes here
        ClearBackground(BLACK);
        sceneManager.Draw();
        EndTextureMode();

        BeginDrawing();
        DrawTexturePro(frameBuffer.texture, { 0, 0, (float)frameBuffer.texture.width, -(float)frameBuffer.texture.height },
            { 0, 0, (float)GetMonitorWidth(GetCurrentMonitor()), (float)GetMonitorHeight(GetCurrentMonitor()) }, { 0, 0 }, 0, WHITE);

        rlImGuiBegin();

        sceneManager.DrawImGui();

        rlImGuiEnd();

        EndDrawing();
    }
}

}

// void Application::Draw()
// {
//     BeginTextureMode(frameBuffer);
//     // textures and stuff goes here
//     ClearBackground(BLACK);
//     sceneManager.Draw();
//     EndTextureMode();
//
//     BeginDrawing();
//     DrawTexturePro(frameBuffer.texture, { 0, 0, (float)frameBuffer.texture.width, -(float)frameBuffer.texture.height },
//         { 0, 0, (float)GetMonitorWidth(GetCurrentMonitor()), (float)GetMonitorHeight(GetCurrentMonitor()) }, { 0, 0 }, 0, WHITE);
//     EndDrawing();
// }

// void Application::Init()
// {
//
//     frameBuffer = LoadRenderTexture(GetMonitorWidth(GetCurrentMonitor()), GetMonitorHeight(GetCurrentMonitor()));
//
//     ToggleFullscreen();
//
//     std::shared_ptr<TitleScreenScene> splashScreen = std::make_shared<TitleScreenScene>(sceneManager);
//     // std::shared_ptr<DevTestScene> gameScene = std::make_shared<DevTestScene>(sceneManager);
//     //
//     unsigned int splashScreenID = sceneManager.Add(splashScreen);
//     // unsigned int gameSceneID = sceneManager.Add(gameScene);
//     //
//     // splashScreen->SetSwitchToScene(gameSceneID);
//
//     sceneManager.SwitchTo(splashScreenID);
// }

// void Application::SetupWindow()
// {
//
//     int monitor = GetCurrentMonitor();
//
//     SetConfigFlags(FLAG_VSYNC_HINT);
//     InitWindow(GetMonitorWidth(monitor), GetMonitorHeight(monitor), "Tabby InDev");
//
//     int maxHeight = GetMonitorHeight(monitor) - 40;
//     if (GetScreenHeight() > maxHeight)
//         SetWindowSize(GetScreenWidth(), maxHeight);
//
//     SetExitKey(0);
//     SetTargetFPS(60);
// }

// void Application::Clean()
// {
//     UnloadRenderTexture(frameBuffer);
//     CloseWindow();
// }