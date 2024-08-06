#pragma once

#include "Tabby/Renderer/UniformBuffer.h"

namespace Tabby {

class OpenGLES3UniformBuffer : public UniformBuffer {
public:
    OpenGLES3UniformBuffer(uint32_t size, uint32_t binding);
    virtual ~OpenGLES3UniformBuffer();

    virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

private:
    uint32_t m_RendererID = 0;
};
}
