#pragma once

#include <Tabby/Asset/AssetBase.h>
#include <Tabby/Renderer/Texture.h>

#include "Tabby/Core/Base.h"
#include "Tabby/Renderer/Texture.h"

namespace Tabby {

struct MSDFData;

class Font : public AssetBase {
public:
    Font(const std::filesystem::path& font);
    ~Font();

    const MSDFData* GetMSDFData() const { return m_Data; }
    Shared<Texture> GetAtlasTexture() const { return m_AtlasTexture; }

    static Shared<Font> GetDefault();
    void Destroy() override;

private:
    MSDFData* m_Data;
    Shared<Texture> m_AtlasTexture;
};

}

// #include <freetype/freetype.h>
//
// namespace Tabby {
//
// struct FontSpecification {
//     std::filesystem::path path;
//     int glyphSize = 48;
// };
//
// struct Character {
//     Shared<Texture> Texture; // The glyph texture
//     glm::ivec2 Size; // Size of glyph
//     glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
//     unsigned int Advance; // Horizontal offset to advance to next glyph
//
//     void Destroy()
//     {
//         Texture->Destroy();
//     }
// };
//
// class Font : public AssetBase {
// public:
//     static Shared<Font> GetDefault();
//
//     Font(const FontSpecification& specification, AssetHandle handle, Buffer data = Buffer());
//     ~Font();
//
//     Shared<Character> GetCharacter(char character);
//
//     void Destroy() override;
//
// private:
//     FT_Face m_Face;
//     FontSpecification m_Specification;
//     std::unordered_map<char, Shared<Character>> m_Characters;
// };
//
// }
