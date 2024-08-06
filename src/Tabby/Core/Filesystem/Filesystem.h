#pragma once

#include <Tabby/Foundation/Types.h>

#include "File.h"
#include <filesystem>

namespace Tabby {

enum class FileReadingFlags : uint32_t {
    READ_BINARY = BIT(0)
};

class FileSystem {
public:
    static Shared<File> ReadFile(std::filesystem::path path, const BitMask& flags);
    static void WriteFile(Shared<File> file, std::filesystem::path path, const BitMask& flags);
    static uint64_t FileSize(std::filesystem::path path);

    static bool CheckDirectory(std::filesystem::path path);
    static bool CreateDirectory(std::filesystem::path path);
    static bool DestroyDirectory(std::filesystem::path path);

    static void SetWorkingDirectory(std::filesystem::path path);
    static std::filesystem::path GetWorkingDirectory();
};

}
