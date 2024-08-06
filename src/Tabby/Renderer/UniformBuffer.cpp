#include "UniformBuffer.h"
#include "tbpch.h"

#include "Drivers/gl46/OpenGL46UniformBuffer.h"
#include "Drivers/gl33/OpenGL33UniformBuffer.h"
#include "Drivers/gles3/OpenGLES3UniformBuffer.h"
#include "Tabby/Renderer/Renderer.h"

namespace Tabby {

Shared<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT_TAGGED(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46UniformBuffer>(size, binding);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33UniformBuffer>(size, binding);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3UniformBuffer>(size, binding);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

}
