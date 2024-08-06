#include "OpenGL33UniformBuffer.h"
#include "tbpch.h"

#include <gl.h>

namespace Tabby {

OpenGL33UniformBuffer::OpenGL33UniformBuffer(uint32_t size, uint32_t binding)
{
    TB_PROFILE_SCOPE();
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGL33UniformBuffer::~OpenGL33UniformBuffer()
{
    TB_PROFILE_SCOPE();
    glDeleteBuffers(1, &m_RendererID);
}

void OpenGL33UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    TB_PROFILE_SCOPE();
    glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

}
