#pragma once

#include <Tabby/Asset/AssetType.h>

namespace Tabby {

struct AssetFileHeader {

    uint8_t header_size = 0;
    AssetType asset_type = AssetType::UNKNOWN;
    uint64_t uncompressed_data_size = 0;
    uint64_t subresources_size = 0;
    uint64_t additional_data = 0;
};

struct AssetFileSubresourceMetadata {
    uint32_t offset = 0;
    uint32_t compressed = 0;
    uint32_t size = 0;
    uint32_t decompressed_size = 0;
};

struct AssetFile {
    AssetFileHeader header;
    std::array<AssetFileSubresourceMetadata, 16> subresources_metadata;
    std::byte* subresources_data;

    AssetFile()
    {
        header = {};
        subresources_data = nullptr;
    }

    ~AssetFile() { Release(); }

    void Release()
    {
        header = {};
        subresources_metadata.fill({});
        delete[] subresources_data;
    }
};

inline std::ostream& operator<<(std::ostream& stream, const AssetFile& file)
{
    stream.write((char*)&file.header, sizeof(AssetFileHeader));
    stream.write((char*)file.subresources_metadata.data(), sizeof(AssetFileSubresourceMetadata) * file.subresources_metadata.size());
    stream.write((char*)file.subresources_data, file.header.subresources_size);

    return stream;
}

}
