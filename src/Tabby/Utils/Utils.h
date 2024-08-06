#pragma once

namespace Tabby {

namespace Utils {

    inline const std::string EvaluateDatetime()
    {
        std::string datetime;

        time_t t;
        struct tm* tt;
        time(&t);
        tt = localtime(&t);

        datetime += std::to_string(tt->tm_year + 1900);

        if (tt->tm_mon < 10)
            datetime += "0";

        datetime += std::to_string(tt->tm_mon + 1);

        if (tt->tm_mday < 10)
            datetime += "0";

        datetime += std::to_string(tt->tm_mday);

        if (tt->tm_hour < 10)
            datetime += "0";

        datetime += std::to_string(tt->tm_hour);

        if (tt->tm_min < 10)
            datetime += "0";

        datetime += std::to_string(tt->tm_min);

        return datetime;
    }

    // Thanks to https://github.com/SaschaWillems for this alignment function
    inline uint64_t Align(uint64_t original, uint64_t min_alignment)
    {
        size_t alignedSize = original;
        if (min_alignment > 0) {
            alignedSize = (alignedSize + min_alignment - 1) & ~(min_alignment - 1);
        }
        return alignedSize;
    }

    inline std::string FormatAllocationSize(uint64_t size)
    {
        if (size / 1'000'000'000 >= 1)
            return fmt::format("{:.4}(GiB)", (double)size / 1'000'000'000.0);
        else if (size / 1'000'000 >= 1)
            return fmt::format("{:.4}(MiB)", (double)size / 1'000'000.0);
        else if (size / 1'000 >= 1)
            return fmt::format("{:.4}(KiB)", (double)size / 1'000.0);
        else
            return fmt::format("{}(B)", size);
    }

    // inline void DecomposeMatrix(const Matrix4& source, glm::vec3* translation, glm::quat* rotation, glm::vec3* scale)
    // {
    //     glm::vec3 skew;
    //     glm::vec4 perpective;
    //
    //     glm::decompose(source, *scale, *rotation, *translation, skew, perpective);
    // };

    // inline Matrix4 ComposeMatrix(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale)
    // {
    //     return glm::translate(Matrix4(1.0f), translation) * Matrix4_cast(rotation) * glm::scale(Matrix4(1.0f), scale);
    // }

    // NOTE: returns amount of mip levels excluding mip #0
    inline uint8_t ComputeNumMipLevelsBC7(uint16_t image_width, uint16_t image_height)
    {
        return log2(std::min(image_width, image_height)) - 2;
    }

    // including mip0 (source image)
    template <uint32_t PixelSize>
    inline uint32_t ComputeMipLevelsStorage(uint16_t image_width, uint16_t image_height)
    {
        uint32_t mip_levels_req = log2(std::min(image_width, image_height)) - 2;

        uint32_t current_width = image_width;
        uint32_t current_height = image_height;

        uint32_t storage_req = current_width * current_height * PixelSize;

        for (int32_t i = 1; i < mip_levels_req + 1; i++) {
            uint32_t req_storage_for_current_mip = (current_width / 2) * (current_height / 2);
            storage_req += req_storage_for_current_mip * PixelSize;
            current_width /= 2;
            current_height /= 2;
        }

        return storage_req;
    }

}

}
