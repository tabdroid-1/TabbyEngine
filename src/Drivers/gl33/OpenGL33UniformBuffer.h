#pragma once

#include "Tabby/Renderer/UniformBuffer.h"

namespace Tabby {

class OpenGL33UniformBuffer : public UniformBuffer {
public:
    OpenGL33UniformBuffer(uint32_t size, uint32_t binding);
    virtual ~OpenGL33UniformBuffer();

    virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

private:
    uint32_t m_RendererID = 0;
};
}
