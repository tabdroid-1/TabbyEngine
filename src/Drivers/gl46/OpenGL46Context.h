#pragma once

#include "Tabby/Renderer/GraphicsContext.h"

struct SDL_Window;

namespace Tabby {

class OpenGL46Context : public GraphicsContext {
public:
    OpenGL46Context(SDL_Window* windowHandle);

    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    SDL_Window* m_WindowHandle;
};

}
