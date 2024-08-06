#include "Drivers/gles3/OpenGLES3Context.h"
#include "tbpch.h"
#include <Drivers/gles3/GLES3.h>

#ifdef TB_PLATFORM_WEB
#define GLAD_PLATFORM_EMSCRIPTEN
#include <emscripten.h>
#elif defined(TB_PLATFORM_ANDROID)
// #include "../../../vendor/SDL2/src/video/SDL_egl.c"
// #include "../../../vendor/SDL2/src/video/SDL_egl_c.h"
// eglGetProcAddress in #include <EGL/egl.h>
#include <SDL_egl.h>
#include <SDL_syswm.h>
#endif

#define GLAD_GLES2_IMPLEMENTATION
#include <gles3.h>

#include <SDL.h>

namespace Tabby {

OpenGLES3Context::OpenGLES3Context(SDL_Window* windowHandle)
    : m_WindowHandle(windowHandle)
{
    TB_CORE_ASSERT_TAGGED(windowHandle, "Window handle is null!");
}

void OpenGLES3Context::Init()
{
    TB_PROFILE_SCOPE();

    SDL_GLContext windowContext = SDL_GL_CreateContext(m_WindowHandle);
    SDL_GL_MakeCurrent(m_WindowHandle, windowContext);

    GladGLES2Context* context = (GladGLES2Context*)calloc(1, sizeof(GladGLES2Context));
    TB_CORE_ASSERT_TAGGED(context, "Failed to initialize GLES3 context!");

#if !defined(TB_PLATFORM_ANDROID)
    int status = gladLoadGLES2Context(context, (GLADloadfunc)SDL_GL_GetProcAddress);
#elif defined(TB_PLATFORM_ANDROID)
    // SDL_SysWMinfo info;
    // SDL_VERSION(&info.version);
    // SDL_bool get_win_info = SDL_GetWindowWMInfo(m_WindowHandle, &info);
    //
    // EGLNativeWindowType eglHWnd = info.info.android.window;
    //
    // EGLint eglConfigAttribList[] = {
    //     EGL_RED_SIZE, 8,
    //     EGL_GREEN_SIZE, 8,
    //     EGL_BLUE_SIZE, 8,
    //     EGL_ALPHA_SIZE, 8 /*: EGL_DONT_CARE*/,
    //     EGL_DEPTH_SIZE, EGL_DONT_CARE,
    //     EGL_STENCIL_SIZE, EGL_DONT_CARE,
    //     EGL_SAMPLE_BUFFERS, 0,
    //     EGL_NONE
    // };
    //
    // EGLint eglSurfaceAttribList[] = {
    //     // EGL_CONTEXT_CLIENT_VERSION, 2,
    //     EGL_NONE
    //
    // };
    // EGLint eglAttribListPbuffer[] = {
    //     EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    //     EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    //     EGL_BUFFER_SIZE, 32,
    //     EGL_DEPTH_SIZE, 24,
    //     EGL_STENCIL_SIZE, 8,
    //     EGL_NONE
    // };
    //
    // EGLint eglNumConfigs;
    // EGLint eglMajorVersion;
    // EGLint minorVersion;
    // EGLDisplay eglDisplay;
    // EGLContext eglContext;
    // EGLSurface eglSurface;
    // EGLConfig eglConfig;
    // EGLint eglContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };
    //
    // eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    // TB_CORE_ASSERT_TAGGED(egDisplay != EGL_NO_DISPLAY, "EGL has no display");
    // // if (egDisplay == EGL_NO_DISPLAY) {
    // //     // return EGL_FALSE;
    // // }
    //
    // // Initialize EGL
    // TB_CORE_ASSERT_TAGGED(eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion), "EGL failed to initialize");
    // // if (!eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion)) {
    // //     // return EGL_FALSE;
    // // }
    //
    // // Get configs
    // TB_CORE_ASSERT_TAGGED(eglGetConfigs(eglDisplay, NULL, 0, &eglNumConfigs), "EGL failed to get configs");
    // // if (!eglGetConfigs(eglDisplay, NULL, 0, &eglNumConfigs)) {
    // //     // return EGL_FALSE;
    // // }
    //
    // TB_CORE_ASSERT_TAGGED(eglChooseConfig(eglDisplay, eglConfigAttribList, &eglConfig, 1, &eglNumConfigs), "EGL failed to choose configs");
    // // if (!eglChooseConfig(eglDisplay, eglConfigAttribList, &eglConfig, 1, &eglNumConfigs)) {
    // //     // return EGL_FALSE;
    // // }
    // //
    // // eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, eglHWnd, eglSurfaceAttribList);
    // // TB_CORE_ASSERT_TAGGED(eglSurface != EGL_NO_SURFACE, "EGL has no surface");
    // // // if (eglSurface == EGL_NO_SURFACE) {
    // // //     // return EGL_FALSE;
    // // // }
    //
    // eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, eglContextAttribs);
    // TB_CORE_ASSERT_TAGGED(eglSurface != EGL_NO_SURFACE, "EGL has no context");
    // // if (context == EGL_NO_CONTEXT) {
    // //     // return EGL_FALSE;
    // // }
    //
    // TB_CORE_ASSERT_TAGGED(eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext), "EGL failed to make current");
    // // if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
    // //     // return EGL_FALSE;
    // // }
    //
    // // SDL_SysWMinfo info;
    // // SDL_VERSION(&info.version);
    // // SDL_bool get_win_info = SDL_GetWindowWMInfo(m_WindowHandle, &info);
    // //
    // // // EGLNativeWindowType eglHWnd = info.info.android.window;
    // // EGLint displayFormat = 0;
    // // eglGetConfigAttrib(platform.device, platform.config, EGL_NATIVE_VISUAL_ID, &displayFormat);

    // int status = gladLoadGLES2Context(context, (GLADloadfunc)SDL_GL_GetProcAddress);
    int status = gladLoadGLES2Context(context, (GLADloadfunc)eglGetProcAddress);
#endif
    TB_CORE_ASSERT_TAGGED(status, "Failed to initialize Glad!");

    GLES3::Init(context);

    TB_CORE_INFO("OpenGL Info:");
    TB_CORE_INFO("  Vendor: {0}", GLES3::GL()->GetString(GL_VENDOR));
    TB_CORE_INFO("  Renderer: {0}", GLES3::GL()->GetString(GL_RENDERER));
    TB_CORE_INFO("  Version: {0}", GLES3::GL()->GetString(GL_VERSION));

    std::string version = reinterpret_cast<const char*>(GLES3::GL()->GetString(GL_VERSION));
    size_t dotPosition = version.find('.');

    int major = std::stoi(version.substr(dotPosition - 1));
    int minor = std::stoi(version.substr(dotPosition + 1));

    TB_CORE_ASSERT_TAGGED(major > 3 || (major == 3 && minor >= 0), "Tabby requires at least OpenGL ES version 3.0!");
}

void OpenGLES3Context::SwapBuffers()
{
    TB_PROFILE_SCOPE();

    SDL_GL_SwapWindow(m_WindowHandle);
}

}
