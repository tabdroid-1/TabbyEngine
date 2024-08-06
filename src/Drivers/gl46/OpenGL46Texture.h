#pragma once

#include "Tabby/Renderer/Texture.h"
#include <gl.h>

namespace Tabby {

class OpenGL46Texture : public Texture {
public:
    OpenGL46Texture(const TextureSpecification& specification, AssetHandle handle, Buffer Data = Buffer());
    virtual ~OpenGL46Texture();

    virtual const TextureSpecification& GetSpecification() const override { return m_Specification; }

    virtual uint32_t GetWidth() const override { return m_Width; }
    virtual uint32_t GetHeight() const override { return m_Height; }
    virtual uint32_t GetRendererID() const override { return m_RendererID; }

    virtual const std::string& GetPath() const override { return m_Path; }

    virtual void SetData(Buffer data) override;
    virtual void SetSubData(void* data, uint32_t width, uint32_t height) override;

    virtual void Bind(uint32_t slot = 0) const override;

    virtual void Destroy() override;

    virtual bool IsLoaded() const override { return m_IsLoaded; }

    virtual bool operator==(const Texture& other) const override
    {
        return m_RendererID == other.GetRendererID();
    }

private:
    TextureSpecification m_Specification;

    std::string m_Path;
    bool m_IsLoaded = false;
    uint32_t m_Width, m_Height;
    uint32_t m_RendererID;
    GLenum m_InternalFormat, m_DataFormat;
};

}
