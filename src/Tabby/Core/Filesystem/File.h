#pragma once

#include <Tabby/Foundation/Types.h>

namespace Tabby {

class File {
public:
    File(byte* data, uint64_t size, BitMask flags, std::filesystem::path path = "")
        : m_Data(data)
        , m_Size(size)
        , m_Path(path)
        , m_Flags(flags)
    {
    }

    void Release() { delete[] m_Data; };

    std::filesystem::path GetPath() const { return m_Path; }
    std::filesystem::path GetName() const { return m_Path.stem(); }
    std::filesystem::path GetFullName() const { return m_Path.filename(); }
    std::filesystem::path GetExtension() const { return m_Path.extension(); }

    byte* GetData() const { return m_Data; };
    uint64_t GetSize() const { return m_Size; };

private:
    std::filesystem::path m_Path;
    byte* m_Data;
    uint64_t m_Size;
    BitMask m_Flags;
};

}
