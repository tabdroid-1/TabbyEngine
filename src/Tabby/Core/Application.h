#pragma once

#include <tbpch.h>
#include "Tabby/Core/Base.h"

#include "Tabby/Core/Events/ApplicationEvent.h"
#include "Tabby/Core/Events/MouseEvent.h"
#include "Tabby/Core/Events/Event.h"
#include "Tabby/Core/Layer/LayerStack.h"
#include "Tabby/Core/Window.h"

#include "Tabby/UI/ImGui/ImGuiLayer.h"

int main(int argc, char** argv);

namespace Tabby {

struct ApplicationCommandLineArgs {
    int Count = 0;
    char** Args = nullptr;

    const char* operator[](int index) const
    {
        TB_CORE_ASSERT(index < Count);
        return Args[index];
    }
};

struct ApplicationSpecification {
    enum class RendererAPI {
        None = 0,
        OpenGL46 = 1,
        OpenGL33 = 2,
        OpenGLES3 = 3
    };
    std::string Name = "Tabby Application";
    std::string WorkingDirectory = "assets";
    bool Resizeable = true;
    bool VSync = true;
    uint32_t Width = 1600;
    uint32_t Height = 900;
    uint32_t MinWidth = 160;
    uint32_t MinHeight = 90;
    float MaxFPS = 0.0f;
    RendererAPI RendererAPI = RendererAPI::OpenGL33;
    ApplicationCommandLineArgs CommandLineArgs;
};

class Application {
public:
    Application(const ApplicationSpecification& specification);
    virtual ~Application();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    Window& GetWindow() { return *m_Window; }

    void Close();

    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }

    static Application& Get() { return *s_Instance; }

    const ApplicationSpecification& GetSpecification() const { return m_Specification; }

    void SubmitToMainThread(const std::function<void()>& function);

    void Run();

private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

    void ExecuteMainThreadQueue();

private:
    ApplicationSpecification m_Specification;
    Scope<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    LayerStack m_LayerStack;

    double m_LastFrameTime = 0.0f;
    bool m_Minimized = false;
    bool m_Running = true;

    std::vector<std::function<void()>> m_MainThreadQueue;
    std::mutex m_MainThreadQueueMutex;

private:
    static Application* s_Instance;
    friend int ::main(int argc, char** argv);
};

// To be defined in CLIENT
Application* CreateApplication(ApplicationCommandLineArgs args);
}
