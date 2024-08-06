#include "Drivers/gl46/OpenGL46Context.h"
#include "tbpch.h"

#include <tracy/TracyOpenGL.hpp>

// #if !defined(TB_PLATFORM_WEB) && !defined(TB_PLATFORM_ANDROID)
#define GLAD_GL_IMPLEMENTATION
#include <gl.h>

#include <SDL2/SDL.h>

namespace Tabby {

OpenGL46Context::OpenGL46Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT_TAGGED(windowHandle, "Window handle is null!");
}

void OpenGL46Context::Init()
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

    TB_CORE_ASSERT_TAGGED(major > 4 || (major == 4 && minor >= 6), "This device does not support OpenGL 4.6!");
}

void OpenGL46Context::SwapBuffers()
{
    TB_PROFILE_SCOPE();

    SDL_GL_SwapWindow(m_WindowHandle);

    // TracyGpuCollect;
}

}
// #endif
