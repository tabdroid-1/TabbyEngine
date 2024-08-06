#include "Tabby/Renderer/Shader.h"
#include "tbpch.h"

#include "Drivers/gl46/OpenGL46Shader.h"
#include "Drivers/gl33/OpenGL33Shader.h"
#include "Drivers/gles3/OpenGLES3Shader.h"
#include "Tabby/Renderer/Renderer.h"

namespace Tabby {

Shared<Shader> Shader::Create(const std::string& filepath)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT_TAGGED(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46Shader>(filepath);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33Shader>(filepath);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3Shader>(filepath);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

Shared<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
{
    switch (Renderer::GetAPI()) {
    case RendererAPI::API::None:
        TB_CORE_ASSERT_TAGGED(false, "RendererAPI::None is currently not supported!");
        return nullptr;
    case RendererAPI::API::OpenGL46:
        return CreateShared<OpenGL46Shader>(name, vertexSrc, fragmentSrc);
    case RendererAPI::API::OpenGL33:
        return CreateShared<OpenGL33Shader>(name, vertexSrc, fragmentSrc);
    case RendererAPI::API::OpenGLES3:
        return CreateShared<OpenGLES3Shader>(name, vertexSrc, fragmentSrc);
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown RendererAPI!");
    return nullptr;
}

void ShaderLibrary::Add(const std::string& name, const Shared<Shader>& shader)
{
    TB_CORE_ASSERT_TAGGED(!Exists(name), "Shader already exists!");
    m_Shaders[name] = shader;
}

void ShaderLibrary::Add(const Shared<Shader>& shader)
{
    auto& name = shader->GetName();
    Add(name, shader);
}

Shared<Shader> ShaderLibrary::Load(const std::string& filepath)
{
    auto shader = Shader::Create(filepath);
    Add(shader);
    return shader;
}

Shared<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
    auto shader = Shader::Create(filepath);
    Add(name, shader);
    return shader;
}

Shared<Shader> ShaderLibrary::Get(const std::string& name)
{
    TB_CORE_ASSERT_TAGGED(Exists(name), "Shader not found!");
    return m_Shaders[name];
}

bool ShaderLibrary::Exists(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}

}
