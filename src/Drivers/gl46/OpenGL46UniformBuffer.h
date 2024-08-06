#pragma once

#include "Tabby/Renderer/UniformBuffer.h"

namespace Tabby {

class OpenGL46UniformBuffer : public UniformBuffer {
public:
    OpenGL46UniformBuffer(uint32_t size, uint32_t binding);
    virtual ~OpenGL46UniformBuffer();

    virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

private:
    uint32_t m_RendererID = 0;
};
}
