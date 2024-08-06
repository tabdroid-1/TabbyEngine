#pragma once

#include "Tabby/Renderer/RendererAPI.h"

namespace Tabby {

class OpenGL46RendererAPI : public RendererAPI {
public:
    virtual void Init() override;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
    virtual void DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount) override;

    virtual void EnableDepthTest(bool enable) override;
    virtual void EnableAlphaBlending(bool enable) override;

    virtual void SetLineWidth(float width) override;
};

}
