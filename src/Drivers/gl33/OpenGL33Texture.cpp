#include "Drivers/gl33/OpenGL33Texture.h"

#include "tbpch.h"
#include <stb_image.h>

namespace Tabby {

namespace Utils {

    static GLenum TabbyImageFormatToGLDataFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB;
        case ImageFormat::RGBA8:
            return GL_RGBA;
        case ImageFormat::RGBA32_UNORM:
            return GL_RGB32UI;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

    static GLenum TabbyImageFormatToGLInternalFormat(ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGB8:
            return GL_RGB8;
        case ImageFormat::RGBA8:
            return GL_RGBA8;
        case ImageFormat::RGBA32_UNORM:
            return GL_RGB32UI;
        }

        TB_CORE_ASSERT(false);
        return 0;
    }

}

OpenGL33Texture::OpenGL33Texture(const TextureSpecification& specification, AssetHandle handle, Buffer data)
    : Texture(0)
    , m_Specification(specification)
    , m_Width(m_Specification.Width)
    , m_Height(m_Specification.Height)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Constructor");

    Handle = handle;
    m_InternalFormat = Utils::TabbyImageFormatToGLInternalFormat(m_Specification.Format);
    m_DataFormat = Utils::TabbyImageFormatToGLDataFormat(m_Specification.Format);

    glPixelStorei(GL_UNPACK_ALIGNMENT, m_Specification.UnpackAlignment);

    glGenTextures(1, &m_RendererID);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);

    glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    if (m_Specification.GenerateMips) {

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    if (data)
        SetData(data);
}

OpenGL33Texture::~OpenGL33Texture()
{
    Destroy();
}

void OpenGL33Texture::SetData(Buffer data)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Set data");

    // uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
    // TB_CORE_ASSERT_TAGGED(data.Size == m_Width * m_Height * bpp, "Data must be the entire texture!");

    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data.Data);

    if (m_Specification.GenerateMips)
        glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenGL33Texture::SetSubData(void* data, uint32_t width, uint32_t height)
{
    TB_PROFILE_SCOPE_NAME("(Texture) Set sub data");

    glBindTexture(GL_TEXTURE_2D, m_RendererID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_DataFormat, GL_UNSIGNED_BYTE, data);

    if (m_Specification.GenerateMips)
        glGenerateMipmap(GL_TEXTURE_2D);
}

void OpenGL33Texture::Bind(uint32_t slot) const
{
    TB_PROFILE_SCOPE_NAME("(Texture) bind");

    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void OpenGL33Texture::Destroy()
{
    TB_PROFILE_SCOPE_NAME("(Textures) Delete");

    glDeleteTextures(1, &m_RendererID);
}
}
