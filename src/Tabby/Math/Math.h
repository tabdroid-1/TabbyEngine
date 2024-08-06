#pragma once

#include <glm/glm.hpp>
#include <Tabby/Foundation/Types.h>

namespace Tabby::Math {

const float PI = 3.14159265358979323846f;
const float DEG2RAD = (PI / 180.0f);
const float RAD2DEG = (180.0f / PI);
const float EPSILON = 0.000001f;

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

template <typename T>
inline bool IsPowerOf2(T value)
{
    if (!std::is_integral_v<T>)
        return false;

    bool result = value && ((value & (value - 1)) == 0);
    return result;
}

inline float Abs(float f)
{
    if (f > 0)
        return f;
    else
        return f * -1;
}

inline float Cross(const Vector2& a, const Vector2& b)
{
    return a.x * b.y - a.y * b.x;
}

inline float Dot(const Vector2& a, const Vector2& b)
{
    return a.x * b.x + a.y * b.y;
}

inline void Normalize(Vector2& v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);

    if (len > EPSILON) {
        float invLen = 1.0f / len;
        v.x *= invLen;
        v.y *= invLen;
    }
}
}
