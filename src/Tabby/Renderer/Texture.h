#pragma once

#include "Tabby/Core/Base.h"
#include <Tabby/Asset/AssetBase.h>

#include <Tabby/Foundation/Types.h>
#include <string>

namespace Tabby {

enum class ImageLayout {
    UNDEFINED = 0,
    GENERAL = 1,
    COLOR_ATTACHMENT = 2,
    DEPTH_STENCIL_ATTACHMENT = 3,
    DEPTH_STENCIL_READ_ONLY = 4,
    SHADER_READ_ONLY = 5,
    TRANSFER_SRC = 6,
    TRANSFER_DST = 7,
    PRESENT_SRC = 1000001002
};

enum class ImageUsage : uint8_t {
    TEXTURE,
    RENDER_TARGET,
    DEPTH_BUFFER
};
// enum class ImageFormat : uint8_t {
//     R8,
//     RB16,
//     RGB24,
//     RGBA32_SRGB,
//     RGBA32_UNORM,
//     BGRA32_SRGB,
//     BGRA32_UNORM,
//     RGB32_HDR,
//     RGBA64_HDR,
//     RGBA128_HDR,
//     D32,
//     BC1,
//     BC5,
//     BC6h,
//     BC7,
//     RGBA64_SFLOAT,
//     RGB24_UNORM
// };

enum class ImageFormat : uint8_t {
    None = 0,
    R8,
    RGB8,
    RGBA8,
    RGBA32F,

    RB16,
    RGB24,
    RGBA32_SRGB,
    RGBA32_UNORM,
    BGRA32_SRGB,
    BGRA32_UNORM,
    RGB32_HDR,
    RGBA64_HDR,
    RGBA128_HDR,
    D32,
    BC1,
    BC5,
    BC6h,
    BC7,
    RGBA64_SFLOAT,
    RGB24_UNORM
};

enum class ImageType : uint8_t {
    TYPE_1D,
    TYPE_2D,
    TYPE_3D,
};

struct TextureSpecification {
    std::filesystem::path path;
    uint32_t Width = 1;
    uint32_t Height = 1;
    ImageFormat Format = ImageFormat::RGBA8;
    ImageUsage usage = ImageUsage::TEXTURE;
    ImageType type = ImageType::TYPE_2D;
    uint8_t array_layers = 1;
    uint32_t UnpackAlignment = 4;
    bool GenerateMips = true;
};

class Texture : public AssetBase {
public:
    static Shared<Texture> Create(const TextureSpecification& specification, AssetHandle handle, Buffer data = Buffer());

    virtual ~Texture() = default;

    virtual const TextureSpecification& GetSpecification() const = 0;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetRendererID() const = 0;

    virtual const std::string& GetPath() const = 0;

    virtual void SetData(Buffer data) = 0;
    virtual void SetSubData(void* data, uint32_t width, uint32_t height) = 0;

    virtual void Bind(uint32_t slot = 0) const = 0;

    virtual bool IsLoaded() const = 0;

    virtual void Destroy() override = 0;
    virtual bool operator==(const Texture& other) const = 0;

protected:
    Texture(AssetHandle handle)
    {
        Handle = handle;
        Type = AssetType::TABBY_IMAGE;
    };
};
}
