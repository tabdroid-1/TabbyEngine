#include "Drivers/gl46/OpenGL46Buffer.h"

#include "tbpch.h"

#include <gl.h>

namespace Tabby {

/////////////////////////////////////////////////////////////////////////////
// VertexBuffer /////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL46VertexBuffer::OpenGL46VertexBuffer(uint32_t size)
{
    TB_PROFILE_SCOPE();

    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

OpenGL46VertexBuffer::OpenGL46VertexBuffer(float* vertices, uint32_t size)
{
    TB_PROFILE_SCOPE();

    glCreateBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

OpenGL46VertexBuffer::~OpenGL46VertexBuffer()
{
    TB_PROFILE_SCOPE();

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL46VertexBuffer::Bind() const
{
    TB_PROFILE_SCOPE();

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void OpenGL46VertexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGL46VertexBuffer::SetData(const void* data, uint32_t size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void OpenGL46VertexBuffer::SetSubData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE_NAME("(VertexBuffer) Add Data");

    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
}

/////////////////////////////////////////////////////////////////////////////
// IndexBuffer //////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

OpenGL46IndexBuffer::OpenGL46IndexBuffer(uint32_t* indices, uint32_t count)
    : m_Count(count)
{
    TB_PROFILE_SCOPE();

    glCreateBuffers(1, &m_RendererID);

    // GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
    // Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state.
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}

OpenGL46IndexBuffer::~OpenGL46IndexBuffer()
{
    TB_PROFILE_SCOPE();

    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL46IndexBuffer::Bind() const
{
    TB_PROFILE_SCOPE();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void OpenGL46IndexBuffer::Unbind() const
{
    TB_PROFILE_SCOPE();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

}
