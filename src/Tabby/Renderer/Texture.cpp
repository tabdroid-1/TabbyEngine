#include "Tabby/Renderer/Texture.h"
#include "tbpch.h"

#include "Drivers/gl46/OpenGL46Texture.h"
#include "Drivers/gl33/OpenGL33Texture.h"
#include "Drivers/gles3/OpenGLES3Texture.h"
#include "Tabby/Renderer/Renderer.h"
#include <Tabby/Foundation/Types.h>

namespace Tabby {

Shared<Texture> Texture::Create(const TextureSpecification& specification, AssetHandle handle, Buffer data)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT_TAGGED(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46Texture>(specification, handle, data);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33Texture>(specification, handle, data);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3Texture>(specification, handle, data);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

// Shared<Texture> Texture::Create(const std::string& path)
// {
//     switch (Renderer::GetAPI()) {
//     case RendererAPI::API::None:
//         TB_CORE_ASSERT_TAGGED(false, "RendererAPI::None is currently not supported!");
//         return nullptr;
//     case RendererAPI::API::OpenGL33:
//         return CreateShared<OpenGL33Texture>(path);
//     case RendererAPI::API::OpenGLES3:
//         return CreateShared<OpenGLES3Texture>(path);
//     }
//
//     TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
//     return nullptr;
// }

}
