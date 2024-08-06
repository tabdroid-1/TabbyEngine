#pragma once

#include "Tabby/Renderer/RendererAPI.h"

namespace Tabby {

class OpenGLES3RendererAPI : public RendererAPI {
public:
    virtual void Init() override;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
    virtual void DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount) override;

    void EnableDepthTest(bool enable);
    void EnableAlphaBlending(bool enable);

    virtual void SetLineWidth(float width) override;
};

}
