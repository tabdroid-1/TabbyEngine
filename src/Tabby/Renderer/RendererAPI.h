#pragma once

#include "Tabby/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Tabby {

class RendererAPI {
public:
    enum class API {
        None = 0,
        OpenGL46 = 1,
        OpenGL33 = 2,
        OpenGLES3 = 3
    };

public:
    virtual ~RendererAPI() = default;

    virtual void Init() = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void Clear() = 0;

    virtual void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
    virtual void DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount) = 0;

    virtual void EnableDepthTest(bool enable) = 0;
    virtual void EnableAlphaBlending(bool enable) = 0;

    virtual void SetLineWidth(float width) = 0;

    static API GetAPI() { return s_API; }
    static Scope<RendererAPI> Create();

private:
    static API s_API;

    friend class Application;
};

}
