#include <Tabby/Renderer/Material.h>

#include <Tabby/Renderer/Renderer.h>
#include <Tabby/Renderer/Renderer2D.h>
#include <Tabby/Renderer/RenderCommand.h>

namespace Tabby {

Material::Material()
    : m_AmbientColor(1.0f)
    , m_AlbedoColor(1.0f)
    , m_AlbedoMapOffset(0)
    , m_AlbedoMapTiling(1)
    , m_MetallicColor(1.0f)
    , m_MetallicMapOffset(0)
    , m_MetallicMapTiling(1)
    , m_NormalMapOffset(0)
    , m_NormalMapTiling(1)
    , m_BumpValue(1.0f)
    , m_DepthTestEnabled(true)
    , m_AlphaBlendingEnabled(true)
{
    m_ModelMatrix = Matrix4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, Vector3(0.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, Vector3(1.0f, 1.0f, 1.0f));
}

Material::Material(const std::string& name, Shared<Shader> shader)
    : m_Name(name)
    , m_Shader(shader)
    , m_AmbientColor(1.0f)
    , m_AlbedoColor(1.0f)
    , m_AlbedoMapOffset(0)
    , m_AlbedoMapTiling(1)
    , m_MetallicColor(1.0f)
    , m_MetallicMapOffset(0)
    , m_MetallicMapTiling(1)
    , m_NormalMapOffset(0)
    , m_NormalMapTiling(1)
    , m_BumpValue(1.0f)
    , m_DepthTestEnabled(true)
    , m_AlphaBlendingEnabled(true)
{
    m_ModelMatrix = Matrix4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, Vector3(0.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, Vector3(1.0f, 1.0f, 1.0f));
}

Material::Material(const std::string& name, const std::string& shaderPath)
    : m_Name(name)
    , m_AmbientColor(1.0f)
    , m_AlbedoColor(1.0f)
    , m_AlbedoMapOffset(0)
    , m_AlbedoMapTiling(1)
    , m_MetallicColor(1.0f)
    , m_MetallicMapOffset(0)
    , m_MetallicMapTiling(1)
    , m_NormalMapOffset(0)
    , m_NormalMapTiling(1)
    , m_BumpValue(1.0f)
    , m_DepthTestEnabled(true)
    , m_AlphaBlendingEnabled(true)
{
    m_Shader = Shader::Create(shaderPath);

    m_ModelMatrix = Matrix4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, Vector3(0.0f, 0.0f, 0.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, Vector3(1.0f, 1.0f, 1.0f));
}

Material::Material(const Shared<Material>& material)
{
    this->m_Name = material->m_Name;
    this->m_Shader = material->m_Shader;

    this->m_AmbientColor = material->m_AmbientColor;

    this->m_AlbedoColor = material->m_AlbedoColor;
    this->m_AlbedoMap = material->m_AlbedoMap;
    this->m_AlbedoMapOffset = material->m_AlbedoMapOffset;
    this->m_AlbedoMapTiling = material->m_AlbedoMapTiling;

    this->m_MetallicColor = material->m_MetallicColor;
    this->m_MetallicMap = material->m_MetallicMap;
    this->m_MetallicMapOffset = material->m_MetallicMapOffset;
    this->m_MetallicMapTiling = material->m_MetallicMapTiling;

    this->m_NormalMap = material->m_NormalMap;
    this->m_NormalMapOffset = material->m_NormalMapOffset;
    this->m_NormalMapTiling = material->m_NormalMapTiling;
    this->m_BumpValue = material->m_BumpValue;

    this->m_AlphaBlendingEnabled = material->m_AlphaBlendingEnabled;
    this->m_DepthTestEnabled = material->m_DepthTestEnabled;
}

void Material::CloneMaterialProperties(Shared<Material> material)
{
    this->m_Name = material->m_Name;
    this->m_Shader = material->m_Shader;

    this->m_AmbientColor = material->m_AmbientColor;

    this->m_AlbedoColor = material->m_AlbedoColor;
    this->m_AlbedoMap = material->m_AlbedoMap;
    this->m_AlbedoMapOffset = material->m_AlbedoMapOffset;
    this->m_AlbedoMapTiling = material->m_AlbedoMapTiling;

    this->m_MetallicColor = material->m_MetallicColor;
    this->m_MetallicMap = material->m_MetallicMap;
    this->m_MetallicMapOffset = material->m_MetallicMapOffset;
    this->m_MetallicMapTiling = material->m_MetallicMapTiling;

    this->m_NormalMap = material->m_NormalMap;
    this->m_NormalMapOffset = material->m_NormalMapOffset;
    this->m_NormalMapTiling = material->m_NormalMapTiling;
    this->m_BumpValue = material->m_BumpValue;

    this->m_AlphaBlendingEnabled = material->m_AlphaBlendingEnabled;
    this->m_DepthTestEnabled = material->m_DepthTestEnabled;
}

void Material::Render()
{
    RenderCommand::EnableDepthTest(m_DepthTestEnabled);
    RenderCommand::EnableAlphaBlending(m_AlphaBlendingEnabled);

    if (m_Shader) {
        m_Shader->Bind();
        m_Shader->SetMat4("u_Model", m_ModelMatrix);
        m_Shader->SetFloat4("u_AmbientColor", m_AmbientColor);
        m_Shader->SetFloat4("u_AlbedoColor", m_AlbedoColor);
        m_Shader->SetFloat4("u_MetallicColor", m_MetallicColor);

        if (m_AlbedoMap) {
            m_AlbedoMap->Bind();
            m_Shader->SetBool("u_HasAlbedoTexture", true);
            m_Shader->SetFloat2("u_AlbedoMapOffset", m_AlbedoMapOffset);
            m_Shader->SetFloat2("u_AlbedoMapTiling", m_AlbedoMapTiling);
        } else {
            m_Shader->SetBool("u_HasAlbedoTexture", false);
        }
    }
}
}
