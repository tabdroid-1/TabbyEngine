#pragma once

#include "Tabby/Core/Base.h"

namespace Tabby {

class UniformBuffer {
public:
    virtual ~UniformBuffer() { }
    virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

    static Shared<UniformBuffer> Create(uint32_t size, uint32_t binding);
};

}
