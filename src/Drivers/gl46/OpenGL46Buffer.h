#pragma once

#include "Tabby/Renderer/Buffer.h"

namespace Tabby {

class OpenGL46VertexBuffer : public VertexBuffer {
public:
    OpenGL46VertexBuffer(uint32_t size);
    OpenGL46VertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGL46VertexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetData(const void* data, uint32_t size) override;
    virtual void SetSubData(const void* data, uint32_t size, uint32_t offset) override;

    virtual const BufferLayout& GetLayout() const override { return m_Layout; }
    virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

private:
    uint32_t m_RendererID;
    BufferLayout m_Layout;
};

class OpenGL46IndexBuffer : public IndexBuffer {
public:
    OpenGL46IndexBuffer(uint32_t* indices, uint32_t count);
    virtual ~OpenGL46IndexBuffer();

    virtual void Bind() const;
    virtual void Unbind() const;

    virtual uint32_t GetCount() const { return m_Count; }

private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};

}
