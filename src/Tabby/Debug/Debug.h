#pragma once

namespace Tabby {

class Scene;
class Debug {
public:
    Debug() { }

    static void DrawRay(const glm::vec3& origin, const glm::vec3& direction, float length = 1.0f, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f });
    static void DrawLine(const glm::vec3& point1, const glm::vec3& point2, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f });
    static void DrawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f });
    static void DrawCircle(const glm::vec3& position, const glm::vec2& size, float thickness = 0.07f, const glm::vec4& color = { 0.0f, 1.0f, 0.0f, 1.0f });

private:
    static void ProcessDrawCalls();

    struct Line {
        glm::vec3 point1;
        glm::vec3 point2;
        glm::vec4 color;
    };

    struct Rect {
        glm::vec3 position;
        glm::vec2 size;
        glm::vec4 color;
    };

    struct Circle {
        glm::vec3 position;
        glm::vec2 size;
        float thickness;
        glm::vec4 color;
    };

    static std::vector<Line> m_Lines;
    static std::vector<Rect> m_Rects;
    static std::vector<Circle> m_Circles;

private:
    friend class World;
};

}
