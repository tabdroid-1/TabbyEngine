#include "Tabby/Renderer/GraphicsContext.h"
#include "tbpch.h"

#include "Drivers/gl46/OpenGL46Context.h"
#include "Drivers/gl33/OpenGL33Context.h"
#include "Drivers/gles3/OpenGLES3Context.h"
#include "Tabby/Renderer/Renderer.h"

namespace Tabby {

Scope<GraphicsContext> GraphicsContext::Create(void* window)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT_TAGGED(false, "No renderer API selected.");
        return nullptr;
    case RendererAPI::API::OpenGL46:
        return CreateScope<OpenGL46Context>(static_cast<SDL_Window*>(window));
    case RendererAPI::API::OpenGL33:
        return CreateScope<OpenGL33Context>(static_cast<SDL_Window*>(window));
    case RendererAPI::API::OpenGLES3:
        return CreateScope<OpenGLES3Context>(static_cast<SDL_Window*>(window));
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
