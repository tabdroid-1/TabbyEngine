#include "Drivers/gl33/OpenGL33Buffer.h"
#include "tbpch.h"

#include <gl.h>

namespace Tabby {

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL33VertexBuffer::OpenGL33VertexBuffer(uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Generate");

    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGL33VertexBuffer::OpenGL33VertexBuffer(float* vertices, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Generate");

    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGL33VertexBuffer::~OpenGL33VertexBuffer()
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Delete");

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL33VertexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Bind");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGL33VertexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Unbind");

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL33VertexBuffer::SetData(const void* data, uint32_t size)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Set Data");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGL33VertexBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Add Data");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL33IndexBuffer::OpenGL33IndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Generate");

    glGenBuffers(1, &m_RendererID);
    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGL33IndexBuffer::~OpenGL33IndexBuffer()
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Delete");

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL33IndexBuffer::Bind() const
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Bind");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGL33IndexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE_NAME("(Index Buffer) Unbind");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
