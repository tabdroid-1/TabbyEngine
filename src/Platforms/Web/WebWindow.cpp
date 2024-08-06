#ifdef TB_PLATFORM_WEB

#include "Platforms/Web/WebWindow.h"
#include "tbpch.h"

#include "Tabby/Core/Input/Input.h"
#include "backends/imgui_impl_sdl2.h"

#include "Tabby/Core/Events/ApplicationEvent.h"
#include "Tabby/Core/Events/KeyEvent.h"
#include "Tabby/Core/Events/MouseEvent.h"

#include "Drivers/gl33/OpenGL33Context.h"
#include "Tabby/Renderer/Renderer.h"

#include <emscripten.h>
#include <emscripten/html5.h>

#include <SDL2/SDL.h>

namespace Tabby {

static uint8_t s_SDLWindowCount = 0;

WebWindow::WebWindow(const WindowProps& props)
{
    TB_PROFILE_SCOPE();

    Init(props);
}

WebWindow::~WebWindow()
{
    TB_PROFILE_SCOPE();

    Shutdown();
}

void WebWindow::Init(const WindowProps& props)
{
    TB_PROFILE_SCOPE();

    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;
    m_Data.MinWidth = props.MinWidth;
    m_Data.MinHeight = props.MinHeight;
    m_Data.Resizeable = props.Resizeable;
    m_Data.VSync = props.VSync;

    TB_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (s_SDLWindowCount == 0) {
        TB_PROFILE_SCOPE_NAME("SDL Init");
        int success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
        if (success != 0) {
            TB_CORE_INFO("{}", SDL_GetError());
        }
        TB_CORE_ASSERT_TAGGED(success == 0, "Could not initialize SDL2!");
    }

    {
        TB_PROFILE_SCOPE_NAME("SDLCreateWindow");
        if (Renderer::GetAPI() == RendererAPI::API::OpenGL33) {
            TB_CORE_ASSERT_TAGGED(false, "OpenGL33 is not supported by EMSCRIPTEN.");
        } else if (Renderer::GetAPI() == RendererAPI::API::OpenGLES3) {

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#if defined(TB_DEBUG)
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
        }
        m_Window = SDL_CreateWindow(
            m_Data.Title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            props.Width, props.Height,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        ++s_SDLWindowCount;
    }

    EmscriptenWebGLContextAttributes attrs;
    attrs.antialias = true;
    attrs.majorVersion = 3;
    attrs.minorVersion = 2;
    attrs.alpha = true;
    attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_DEFAULT;

    // The following lines must be done in exact order, or it will break!
    emscripten_webgl_init_context_attributes(&attrs); // you MUST init the attributes before creating the context
    attrs.majorVersion = 3; // you MUST set the version AFTER the above line
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE webgl_context = emscripten_webgl_create_context("#canvas", &attrs);
    emscripten_webgl_make_context_current(webgl_context);

    m_Context = GraphicsContext::Create(m_Window);
    m_Context->Init();

    SDL_SetWindowData(m_Window, "WindowData", &m_Data);
    SetVSync(props.VSync);
    SetResizable(props.Resizeable);
    SetMinSize(props.MinWidth, props.MinHeight);

    Input::Init();
}

void WebWindow::Shutdown()
{
    TB_PROFILE_SCOPE();

    SDL_DestroyWindow(m_Window);
    --s_SDLWindowCount;

    if (s_SDLWindowCount == 0) {
        SDL_Quit();
    }
}

void WebWindow::OnUpdate()
{
    TB_PROFILE_SCOPE();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
        case SDL_WINDOWEVENT: {
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                m_Data.Width = event.window.data1;
                m_Data.Height = event.window.data2;
                WindowResizeEvent resizeEvent(m_Data.Width, m_Data.Height);
                m_Data.EventCallback(resizeEvent);
            }
            break;
        }

        case SDL_QUIT: {
            WindowCloseEvent closeEvent;
            m_Data.EventCallback(closeEvent);
            break;
        }

        case SDL_KEYDOWN: {
            if (event.key.repeat == 0) {
                KeyPressedEvent keyPressedEvent(event.key.keysym.scancode, false);
                m_Data.EventCallback(keyPressedEvent);
            }
            break;
        }

        case SDL_KEYUP: {
            KeyReleasedEvent keyReleasedEvent(event.key.keysym.scancode);
            m_Data.EventCallback(keyReleasedEvent);
            break;
        }

        case SDL_TEXTINPUT: {
            KeyTypedEvent keyTypedEvent(event.text.text[0]);
            m_Data.EventCallback(keyTypedEvent);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            if (event.button.state == SDL_PRESSED) {
                MouseButtonPressedEvent mouseButtonPressedEvent(event.button.button);
                m_Data.EventCallback(mouseButtonPressedEvent);
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            if (event.button.state == SDL_RELEASED) {
                MouseButtonReleasedEvent mouseButtonReleasedEvent(event.button.button);
                m_Data.EventCallback(mouseButtonReleasedEvent);
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            MouseMovedEvent mouseMovedEvent(static_cast<float>(event.motion.x), static_cast<float>(event.motion.y));
            m_Data.EventCallback(mouseMovedEvent);
            break;
        }

        case SDL_MOUSEWHEEL: {
            MouseScrolledEvent mouseScrolledEvent(static_cast<float>(event.wheel.x), static_cast<float>(event.wheel.y));
            m_Data.EventCallback(mouseScrolledEvent);
            break;
        }
        }
    }

    m_Context->SwapBuffers();
}

void WebWindow::SetVSync(bool enabled)
{
    TB_PROFILE_SCOPE();

    if (enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);

    m_Data.VSync = enabled;
}

bool WebWindow::IsVSync() const
{
    return m_Data.VSync;
}

void WebWindow::SetResizable(bool enabled)
{

    if (enabled)
        SDL_SetWindowResizable(m_Window, SDL_TRUE);
    else
        SDL_SetWindowResizable(m_Window, SDL_FALSE);
    m_Data.Resizeable = enabled;
}

bool WebWindow::GetResizable() const
{
    return m_Data.Resizeable;
}

void WebWindow::SetMinSize(uint32_t minWidth, uint32_t minHeight)
{
    SDL_SetWindowMinimumSize(m_Window, minWidth, minHeight);
    m_Data.MinWidth = minWidth;
    m_Data.MinHeight = minHeight;
}
}

#endif
