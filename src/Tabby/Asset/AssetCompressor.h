#pragma once

#include "AssetBase.h"
#include "AssetFile.h"

#include <tbpch.h>

namespace Tabby {

/*
 *  @brief Singleton for asset compression functionality.
 */
class AssetCompressor {
public:
    /*
     *  @brief Generates mip levels.Discards last two levels with 1 - or 2 - pixel wide dimensions(because mips are further used for BC7 encoding)
     */
    static std::vector<RGBA32> GenerateMipMaps(const std::vector<RGBA32>& mip0_data, uint32_t image_width, uint32_t image_height);

    /*
     *  @brief Encodes RGBA32 image to BC7 image.
     *  @return Array of 128-bit values, representing blocks
     */
    static std::vector<uint32_t> CompressBC7(const std::vector<RGBA32>& source, uint32_t image_width, uint32_t image_height, uint8_t mip_levels_count);

    /*
     *  @brief Compresses data using NVIDIA GDeflate algorithm.
     *  @return Size of compressed data
     *  @param[out] out: data stream to write compressed data in
     */
    static uint32_t CompressGDeflate(const std::vector<uint8_t>& data, std::ostream* out);

    /*
     *  @brief Decompresses data using single core CPU GDeflate decompression implementation.
     *  @param[in] in: input stream of data
     *  @param[out] out: vector of bytes
     */
    static uint64_t DecompressGDeflateCPU(std::istream* in, uint64_t size, std::ostream* out);

    /*
     *  @brief Updates given CRC32 checksum for a given stream of data
     *  @return Updated CRC32 checksum
     */
    static uint32_t GDeflateCRC32(uint32_t crc32, const std::vector<uint32_t>& data);

    inline static constexpr uint32_t GDEFLATE_PAGE_SIZE = 65536;
    inline static constexpr uint32_t GDEFLATE_END_OF_PAGES_TAG = 1;

    struct GDeflatePageHeader {
        size_t compressed_size = 0;
    };

private:
    inline static constexpr GDeflatePageHeader EndOfPagesTag = { 0 };
};

}
