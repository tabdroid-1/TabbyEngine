#include "OpenGLES3UniformBuffer.h"
#include "GLES3.h"
#include "tbpch.h"

#include <gles3.h>

namespace Tabby {

OpenGLES3UniformBuffer::OpenGLES3UniformBuffer(uint32_t size, uint32_t binding)
{
    GLES3::GL()->GenBuffers(1, &m_RendererID);
    GLES3::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    GLES3::GL()->BufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    GLES3::GL()->BindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
    // glGenBuffers(1, &m_RendererID);
    // glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
    // glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
}

OpenGLES3UniformBuffer::~OpenGLES3UniformBuffer()
{
    GLES3::GL()->DeleteBuffers(1, &m_RendererID);
}

void OpenGLES3UniformBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
    // glBufferSubData(m_RendererID, offset, size, data);

    GLES3::GL()->BindBuffer(GL_UNIFORM_BUFFER, m_RendererID);

    // Update the buffer data
    GLES3::GL()->BufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    // glNamedBufferSubData(m_RendererID, offset, size, data);
}

}
