#pragma once

#include <Tabby/Renderer/Shader.h>
#include <Tabby/Renderer/Texture.h>
#include <Tabby/Renderer/Shader.h>

// TODO: use asset handle instead of Texture.

namespace Tabby {

class Material {
public:
    enum TextureType {
        Albedo,
        Metallic,
        Normal,
        Roughness,
        AmbientOcclusion
    };

    Material();
    Material(const std::string& name, Shared<Shader> shader);
    Material(const std::string& name, const std::string& shaderPath);
    Material(const Shared<Material>& material);
    virtual ~Material() = default;

    void CloneMaterialProperties(Shared<Material> material);

    const std::string& GetName() const { return m_Name; }

    // Name
    void SetName(std::string name) { m_Name = name; }

    void SetMatrix(const Matrix4& matrix) { m_ModelMatrix = matrix; }
    Matrix4 GetMatrix() const { return m_ModelMatrix; }

    // Ambient
    void SetAmbientColor(const Vector4& ambientColor) { m_AmbientColor = ambientColor; }
    Vector4 GetAmbientColor() const { return m_AmbientColor; }

    // Albedo
    void SetAlbedoColor(const Vector4& albedoColor) { m_AlbedoColor = albedoColor; }
    void SetAlbedoMap(const Shared<Texture> albedoMap) { m_AlbedoMap = albedoMap; }
    void SetAlbedoMapOffset(Vector2 offset) { m_AlbedoMapOffset = offset; }
    void SetAlbedoMapTiling(Vector2 tiling) { m_AlbedoMapTiling = tiling; }
    Vector4 GetAlbedoColor() const { return m_AlbedoColor; }
    Shared<Texture> GetAlbedoMap() const { return m_AlbedoMap; }
    Vector2 GetAlbedoMapOffset() const { return m_AlbedoMapOffset; }
    Vector2 GetAlbedoMapTiling() const { return m_AlbedoMapTiling; }

    // Normal Map
    void SetNormalMap(const Shared<Texture> normalMap) { m_NormalMap = normalMap; }
    void SetNormalMapOffset(Vector2 offset) { m_NormalMapOffset = offset; }
    void SetNormalMapTiling(Vector2 tiling) { m_NormalMapTiling = tiling; }
    void SetBumpValue(const float bumpValue) { m_BumpValue = bumpValue; }
    Shared<Texture> GetNormalMap() const { return m_NormalMap; }
    Vector2 GetNormalMapOffset() const { return m_NormalMapOffset; }
    Vector2 GetNormalMapTiling() const { return m_NormalMapTiling; }
    float GetBumpValue() const { return m_BumpValue; }

    // Metallic
    void SetMetallicColor(const Vector4& metallicColor) { m_MetallicColor = metallicColor; }
    void SetMetallicMap(const Shared<Texture> metallicMap) { m_MetallicMap = metallicMap; }
    void SetMetallicMapOffset(Vector2 offset) { m_MetallicMapOffset = offset; }
    void SetMetallicMapTiling(Vector2 tiling) { m_MetallicMapTiling = tiling; }
    Vector4 GetMetallicColor() const { return m_MetallicColor; }
    Shared<Texture> GetMetallicMap() const { return m_MetallicMap; }
    Vector2 GetMetallicMapOffset() const { return m_MetallicMapOffset; }
    Vector2 GetMetallicMapTiling() const { return m_MetallicMapTiling; }

    // Ambient Occlusion
    // void SetAmbientOcclusionColor(const Vector4& ambientOcclusionColor) { m_AmbientOcclusionColor = ambientOcclusionColor; }
    void SetAmbientOcclusionMap(const Shared<Texture> ambientOcclusionMap) { m_AmbientOcclusionMap = ambientOcclusionMap; }
    void SetAmbientOcclusionMapOffset(Vector2 offset) { m_AmbientOcclusionMapOffset = offset; }
    void SetAmbientOcclusionMapTiling(Vector2 tiling) { m_AmbientOcclusionMapTiling = tiling; }
    // Vector4 GetAmbientOcclusionColor() const { return m_AmbientOcclussionColor; }
    Shared<Texture> GetAmbientOcclusionMap() const { return m_AmbientOcclusionMap; }
    Vector2 GetAmbientOcclusionMapOffset() const { return m_AmbientOcclusionMapOffset; }
    Vector2 GetAmbientOcclusionMapTiling() const { return m_AmbientOcclusionMapTiling; }

    // Shader
    const Shared<Shader>& GetShader() { return m_Shader; }

    // Other material properties
    void EnableDepthTest() { m_DepthTestEnabled = true; }
    void DisableDepthTest() { m_DepthTestEnabled = false; }
    void EnableAlphaBlending() { m_AlphaBlendingEnabled = true; }
    void DisableAlphaBlending() { m_AlphaBlendingEnabled = false; }

    void Render();

private:
    std::string m_Name;
    Shared<Shader> m_Shader = nullptr;

    Matrix4 m_ModelMatrix;

    // Ambient
    Vector4 m_AmbientColor = Vector4(0.8f, 0.8f, 0.8f, 1);
    // Albedo
    Vector4 m_AlbedoColor = Vector4(0.8f, 0.8f, 0.8f, 1);
    Shared<Texture> m_AlbedoMap = nullptr;
    Vector2 m_AlbedoMapOffset = Vector2(0, 0);
    Vector2 m_AlbedoMapTiling = Vector2(1, 1);
    // Normal
    Shared<Texture> m_NormalMap = nullptr;
    Vector2 m_NormalMapOffset = Vector2(0, 0);
    Vector2 m_NormalMapTiling = Vector2(1, 1);
    float m_BumpValue = 0.0f;
    // Metallic
    Vector4 m_MetallicColor = Vector4(0.8f, 0.8f, 0.8f, 1);
    Shared<Texture> m_MetallicMap = nullptr;
    Vector2 m_MetallicMapOffset = Vector2(0, 0);
    Vector2 m_MetallicMapTiling = Vector2(1, 1);
    // Ambient Occlusion
    Shared<Texture> m_AmbientOcclusionMap = nullptr;
    Vector2 m_AmbientOcclusionMapOffset = Vector2(0, 0);
    Vector2 m_AmbientOcclusionMapTiling = Vector2(1, 1);

    // Other material properties
    bool m_DepthTestEnabled;
    bool m_AlphaBlendingEnabled;
};
}
