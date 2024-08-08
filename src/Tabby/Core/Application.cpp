#include <Tabby/Core/Application.h>
#include "Tabby/Renderer/Renderer.h"
#include <Tabby/Core/Input/Input.h>

#include "Tabby/Utils/PlatformUtils.h"
#include <Tabby/Asset/AssetManager.h>
#include <Tabby/Core/Time/Time.h>
#include <Tabby/Audio/AudioEngine.h>
#include <Tabby/Core/Filesystem/Filesystem.h>

namespace Tabby {

Application* Application::s_Instance = nullptr;

Application::Application(const ApplicationSpecification& specification)
    : m_Specification(specification)
{
    TB_PROFILE_SCOPE();

    TB_CORE_ASSERT_TAGGED(!s_Instance, "Application already exists!");
    s_Instance = this;

    switch (specification.RendererAPI) {
    case ApplicationSpecification::RendererAPI::OpenGL46:
        RendererAPI::s_API = RendererAPI::API::OpenGL46;
        break;
    case ApplicationSpecification::RendererAPI::OpenGL33:
        RendererAPI::s_API = RendererAPI::API::OpenGL33;
        break;
    case ApplicationSpecification::RendererAPI::OpenGLES3:
        RendererAPI::s_API = RendererAPI::API::OpenGLES3;
        break;
    }

    if (!m_Specification.WorkingDirectory.empty()) {
        FileSystem::SetWorkingDirectory(m_Specification.WorkingDirectory);
    }

    m_Window = Window::Create(WindowProps(m_Specification.Name, m_Specification.Width, m_Specification.Height, m_Specification.MinWidth, m_Specification.MinHeight, m_Specification.Resizeable, m_Specification.VSync));
    m_Window->SetEventCallback(TB_BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();
    AssetManager::Init();
    Audio::Engine::Init();
    Input::Init();

    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
    TB_PROFILE_SCOPE();
    Audio::Engine::Shutdown();
    Renderer::Shutdown();

    AssetManager::Get()->FullUnload();
    AssetManager::Shutdown();
}

void Application::PushLayer(Layer* layer)
{
    TB_PROFILE_SCOPE();

    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}

void Application::PushOverlay(Layer* layer)
{
    TB_PROFILE_SCOPE();

    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close()
{
    m_Running = false;
}

void Application::SubmitToMainThread(const std::function<void()>& function)
{
    std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

    m_MainThreadQueue.emplace_back(function);
}

void Application::OnEvent(Event& e)
{
    TB_PROFILE_SCOPE();

    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(TB_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(TB_BIND_EVENT_FN(Application::OnWindowResize));
    dispatcher.Dispatch<MouseScrolledEvent>(TB_BIND_EVENT_FN(Application::OnMouseScroll));

    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
        if (e.Handled)
            break;
        (*it)->OnEvent(e);
    }
}

void Application::Run()
{

#ifdef TB_PLATFORM_WEB
    if (m_Running) {
#else
    while (m_Running) {
#endif
        TB_PROFILE_FRAME("Application");

        double time = Time::GetTime();
        Time::SetDeltaTime(time - m_LastFrameTime);
        m_LastFrameTime = time;

        ExecuteMainThreadQueue();

        if (!m_Minimized && Time::GetDeltaTime() < 200.0f) { // ts < 200  because ts is really high number in first frame
                                                             // and that breaks the phyiscs and some other stuff.
                                                             // this happens because m_LastFrameTime is 0 in first frame.
            {
                TB_PROFILE_SCOPE_NAME("LayerStack OnUpdate");

                // TODO: Remove TimeStep
                for (Layer* layer : m_LayerStack)
                    layer->OnUpdate();
            }

            m_ImGuiLayer->Begin();
            {
                TB_PROFILE_SCOPE_NAME("LayerStack OnImGuiRender");

                for (Layer* layer : m_LayerStack)
                    layer->OnImGuiRender();
            }
            m_ImGuiLayer->End();
        }

        Input::s_Instance->m_MouseScrollDelta = { 0, 0 };
        m_Window->OnUpdate();

        // Framerate limiter. this will do nothing if maxFPS is 0.
        if (m_Specification.MaxFPS > 0.0) {
            double frameTime = Time::GetTime() - time;

            double frameTimeLimit = 1.0 / m_Specification.MaxFPS;
            if (frameTime < frameTimeLimit) {
                double sleepTime = frameTimeLimit - frameTime;
                std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
            }
        }
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e)
{
    TB_PROFILE_SCOPE();

    if (e.GetWidth() == 0 || e.GetHeight() == 0) {
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

bool Application::OnMouseScroll(MouseScrolledEvent& e)
{
    Input::s_Instance->m_MouseScrollDelta = { e.GetXOffset(), e.GetYOffset() };
    return false;
}

void Application::ExecuteMainThreadQueue()
{
    std::scoped_lock<std::mutex> lock(m_MainThreadQueueMutex);

    for (auto& func : m_MainThreadQueue)
        func();

    m_MainThreadQueue.clear();
}
}
