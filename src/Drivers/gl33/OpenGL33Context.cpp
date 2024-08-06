#include "Drivers/gl33/OpenGL33Context.h"

#include "tbpch.h"

#include <tracy/TracyOpenGL.hpp>

// #if !defined(TB_PLATFORM_WEB) && !defined(TB_PLATFORM_ANDROID)
// #define GLAD_GL_IMPLEMENTATION only one is enough (in OpenGL46Context)
#include <gl.h>

#include <SDL.h>

namespace Tabby {

OpenGL33Context::OpenGL33Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT_TAGGED(windowHandle, "Window handle is null!");
}

void OpenGL33Context::Init()
{
    TB_PROFILE_SCOPE();

    SDL_GLContext windowContext = SDL_GL_CreateContext(m_WindowHandle);
    SDL_GL_MakeCurrent(m_WindowHandle, windowContext);

    int status = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
    TB_CORE_ASSERT_TAGGED(status, "Failed to initialize Glad!");

    // TracyGpuContext;

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
    TB_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

    std::string version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    size_t dotPosition = version.find('.');

    int major = std::stoi(version.substr(0, dotPosition));
    int minor = std::stoi(version.substr(dotPosition + 1));

    TB_CORE_ASSERT_TAGGED(major > 3 || (major == 3 && minor >= 3), "Tabby requires at least OpenGL version 3.3!");
}

void OpenGL33Context::SwapBuffers()
{
    TB_PROFILE_SCOPE();

    SDL_GL_SwapWindow(m_WindowHandle);

    // TracyGpuCollect;
}

}
// #endif
