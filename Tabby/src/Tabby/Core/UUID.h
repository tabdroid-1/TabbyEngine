#pragma once

// namespace Tabby {
//
// class UUID {
// public:
//     UUID();
//     UUID(uint64_t uuid);
//     UUID(const UUID&) = default;
//
//     operator uint64_t() const { return m_UUID; }
//
// private:
//     uint64_t m_UUID;
// };
// }
//
// template <>
// struct std::hash<Tabby::UUID> {
//     std::size_t operator()(const Tabby::UUID& uuid) const
//     {
//         return hash<uint64_t>()((uint64_t)uuid);
//     }
// };

#include <tbpch.h>

namespace Tabby {

class UUID {
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID&) = default;

    uint64_t Get() const { return m_UUID; }
    bool Valid() const { return m_UUID != 0; }

    operator uint64_t() const { return m_UUID; }

private:
    uint64_t m_UUID;
};
}

namespace std {
template <>
struct hash<Tabby::UUID> {
    std::size_t operator()(const Tabby::UUID& uuid) const
    {
        return hash<uint64_t>()(static_cast<uint64_t>(uuid));
    }
};
}
