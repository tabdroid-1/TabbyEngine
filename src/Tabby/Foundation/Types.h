#pragma once

#include <tbpch.h>
#include <Tabby/Core/UUID.h>
#include <glm/glm.hpp>

namespace Tabby {

typedef uint8_t byte;

using BitMask = uint64_t;
using Flag = uint64_t;
using AssetHandle = UUID;

// struct MiscData {
//     byte* data;
//     uint32_t size;
// };

// using RGB32         = glm::u8vec3;
// using RGBA32        = glm::u8vec4;

using IntVector2 = glm::vec<2, int32_t>;
using IntVector3 = glm::vec<3, int32_t>;
using IntVector4 = glm::vec<4, int32_t>;

using UIntVector2 = glm::vec<2, uint32_t>;
using UIntVector3 = glm::vec<3, uint32_t>;
using UIntVector4 = glm::vec<4, uint32_t>;

using Vector2 = glm::vec2;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;

using Matrix2 = glm::mat2;
using Matrix3 = glm::mat3;
using Matrix4 = glm::mat4;

using Quaternion = glm::quat;

struct Sphere {
    Vector3 center;
    float radius;
};

struct Circle {
    Vector2 center;
    float radius;
};

struct AABB {
    Vector3 min, max;
};

struct AABB2D {
    Vector2 min, max;
};

struct Bounds {
    Sphere sphere;
    AABB aabb;
};

struct Plane {
    Vector3 normal;
    float distance;

    Plane()
    {
        normal = { 0.0f, 1.0f, 0.0f };
        distance = 0.0f;
    }

    Plane(const Vector3& p1, const Vector3& norm)
        : normal(glm::normalize(norm))
        , distance(glm::dot(normal, p1))
    {
    }
};

class Frustum {
public:
    Frustum() { }

    // m = ProjectionMatrix * ViewMatrix
    Frustum(const Matrix4& ProjectionMatrix, const Matrix4& transform)
    {
        Matrix4 m = ProjectionMatrix * glm::inverse(transform);
        m = glm::transpose(m);
        m_planes[Left] = m[3] + m[0];
        m_planes[Right] = m[3] - m[0];
        m_planes[Bottom] = m[3] + m[1];
        m_planes[Top] = m[3] - m[1];
        m_planes[Near] = m[3] + m[2];
        m_planes[Far] = m[3] - m[2];

        Vector3 crosses[Combinations] = {
            glm::cross(Vector3(m_planes[Left]), Vector3(m_planes[Right])),
            glm::cross(Vector3(m_planes[Left]), Vector3(m_planes[Bottom])),
            glm::cross(Vector3(m_planes[Left]), Vector3(m_planes[Top])),
            glm::cross(Vector3(m_planes[Left]), Vector3(m_planes[Near])),
            glm::cross(Vector3(m_planes[Left]), Vector3(m_planes[Far])),
            glm::cross(Vector3(m_planes[Right]), Vector3(m_planes[Bottom])),
            glm::cross(Vector3(m_planes[Right]), Vector3(m_planes[Top])),
            glm::cross(Vector3(m_planes[Right]), Vector3(m_planes[Near])),
            glm::cross(Vector3(m_planes[Right]), Vector3(m_planes[Far])),
            glm::cross(Vector3(m_planes[Bottom]), Vector3(m_planes[Top])),
            glm::cross(Vector3(m_planes[Bottom]), Vector3(m_planes[Near])),
            glm::cross(Vector3(m_planes[Bottom]), Vector3(m_planes[Far])),
            glm::cross(Vector3(m_planes[Top]), Vector3(m_planes[Near])),
            glm::cross(Vector3(m_planes[Top]), Vector3(m_planes[Far])),
            glm::cross(Vector3(m_planes[Near]), Vector3(m_planes[Far]))
        };

        m_points[0] = intersection<Left, Bottom, Near>(crosses);
        m_points[1] = intersection<Left, Top, Near>(crosses);
        m_points[2] = intersection<Right, Bottom, Near>(crosses);
        m_points[3] = intersection<Right, Top, Near>(crosses);
        m_points[4] = intersection<Left, Bottom, Far>(crosses);
        m_points[5] = intersection<Left, Top, Far>(crosses);
        m_points[6] = intersection<Right, Bottom, Far>(crosses);
        m_points[7] = intersection<Right, Top, Far>(crosses);
    }

    bool IsBoxVisible(const Vector3& minp, const Vector3& maxp) const
    {
        // check box outside/inside of frustum
        for (int i = 0; i < Count; i++) {
            // clang-format off
            if ((glm::dot(m_planes[i], Vector4(minp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0) &&
                (glm::dot(m_planes[i], Vector4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0))
            {
                return false;
            }
            // clang-format on
        }

        // check frustum outside/inside box
        // clang-format off
        int out;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
        out = 0; for (int i = 0; i<8; i++) out += ((m_points[i].z < minp.z) ? 1 : 0); if (out == 8) return false;
        // clang-format on

        return true;
    }

    Vector3* GetPoints() { return m_points; }

private:
    enum Planes {
        Left = 0,
        Right,
        Bottom,
        Top,
        Near,
        Far,
        Count,
        Combinations = Count * (Count - 1) / 2
    };

    template <Planes i, Planes j>
    struct ij2k {
        enum { k = i * (9 - i) / 2 + j - 1 };
    };

    // template <Planes a, Planes b, Planes c>
    // Vector3 intersection(const Vector3* crosses) const;
    template <Frustum::Planes a, Frustum::Planes b, Frustum::Planes c>
    inline Vector3 intersection(const Vector3* crosses) const
    {
        float D = glm::dot(Vector3(m_planes[a]), crosses[ij2k<b, c>::k]);
        Vector3 res = glm::mat3(crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k], crosses[ij2k<a, b>::k]) * Vector3(m_planes[a].w, m_planes[b].w, m_planes[c].w);
        return res * (-1.0f / D);
    }

    Vector4 m_planes[Count];
    Vector3 m_points[8];
};

// Macro table is usually being iterated, so I use an array of pairs and not a map, because map
// fits better for random access
using ShaderMacroTable = std::map<std::string, std::string>;

// Non-owning raw buffer class
struct Buffer {
    uint8_t* Data = nullptr;
    uint64_t Size = 0;

    Buffer() = default;

    Buffer(uint64_t size)
    {
        Allocate(size);
    }

    Buffer(const void* data, uint64_t size)
        : Data((uint8_t*)data)
        , Size(size)
    {
    }

    Buffer(const Buffer&) = default;

    static Buffer Copy(Buffer other)
    {
        Buffer result(other.Size);
        memcpy(result.Data, other.Data, other.Size);
        return result;
    }

    void Allocate(uint64_t size)
    {
        Release();

        Data = (uint8_t*)malloc(size);
        Size = size;
    }

    void Release()
    {
        free(Data);
        Data = nullptr;
        Size = 0;
    }

    template <typename T>
    T* As()
    {
        return (T*)Data;
    }

    operator bool() const
    {
        return (bool)Data;
    }
};

struct ScopedBuffer {
    ScopedBuffer(Buffer buffer)
        : m_Buffer(buffer)
    {
    }

    ScopedBuffer(uint64_t size)
        : m_Buffer(size)
    {
    }

    ~ScopedBuffer()
    {
        m_Buffer.Release();
    }

    uint8_t* Data() { return m_Buffer.Data; }
    uint64_t Size() { return m_Buffer.Size; }

    template <typename T>
    T* As()
    {
        return m_Buffer.As<T>();
    }

    operator bool() const { return m_Buffer; }

private:
    Buffer m_Buffer;
};

}
