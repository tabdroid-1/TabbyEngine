#pragma once

#include "Tabby/Renderer/Buffer.h"

namespace Tabby {

class OpenGL33VertexBuffer : public VertexBuffer {
public:
    OpenGL33VertexBuffer(uint32_t size);
    OpenGL33VertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGL33VertexBuffer();

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

class OpenGL33IndexBuffer : public IndexBuffer {
public:
    OpenGL33IndexBuffer(uint32_t* indices, uint32_t count);
    virtual ~OpenGL33IndexBuffer();

    virtual void Bind() const;
    virtual void Unbind() const;

    virtual uint32_t GetCount() const { return m_Count; }

private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};

}
