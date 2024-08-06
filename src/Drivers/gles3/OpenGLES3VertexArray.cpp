#include "Drivers/gles3/OpenGLES3VertexArray.h"
#include "Drivers/gles3/GLES3.h"
#include "tbpch.h"

#include <gles3.h>

namespace Tabby {

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type) {
    case ShaderDataType::Float:
        return GL_FLOAT;
    case ShaderDataType::Float2:
        return GL_FLOAT;
    case ShaderDataType::Float3:
        return GL_FLOAT;
    case ShaderDataType::Float4:
        return GL_FLOAT;
    case ShaderDataType::Mat3:
        return GL_FLOAT;
    case ShaderDataType::Mat4:
        return GL_FLOAT;
    case ShaderDataType::Int:
        return GL_INT;
    case ShaderDataType::Int2:
        return GL_INT;
    case ShaderDataType::Int3:
        return GL_INT;
    case ShaderDataType::Int4:
        return GL_INT;
    case ShaderDataType::Bool:
        return GL_BOOL;
    }

    TB_CORE_ASSERT_TAGGED(false, "Unknown ShaderDataType!");
    return 0;
}

OpenGLES3VertexArray::OpenGLES3VertexArray()
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->GenVertexArrays(1, &m_RendererID);
}

OpenGLES3VertexArray::~OpenGLES3VertexArray()
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->DeleteVertexArrays(1, &m_RendererID);
}

void OpenGLES3VertexArray::Bind() const
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->BindVertexArray(m_RendererID);
}

void OpenGLES3VertexArray::Unbind() const
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->BindVertexArray(0);
}

void OpenGLES3VertexArray::AddVertexBuffer(const Shared<VertexBuffer>& vertexBuffer)
{
    // TB_PROFILE_FUNCTION();

    TB_CORE_ASSERT_TAGGED(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

    GLES3::GL()->BindVertexArray(m_RendererID);
    vertexBuffer->Bind();

    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout) {
        switch (element.Type) {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4: {
            GLES3::GL()->EnableVertexAttribArray(m_VertexBufferIndex);
            GLES3::GL()->VertexAttribPointer(m_VertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                element.Normalized ? GL_TRUE : GL_FALSE,
                layout.GetStride(),
                (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool: {
            GLES3::GL()->EnableVertexAttribArray(m_VertexBufferIndex);
            GLES3::GL()->VertexAttribIPointer(m_VertexBufferIndex,
                element.GetComponentCount(),
                ShaderDataTypeToOpenGLBaseType(element.Type),
                layout.GetStride(),
                (const void*)element.Offset);
            m_VertexBufferIndex++;
            break;
        }
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4: {
            uint8_t count = element.GetComponentCount();
            for (uint8_t i = 0; i < count; i++) {
                GLES3::GL()->EnableVertexAttribArray(m_VertexBufferIndex);
                GLES3::GL()->VertexAttribPointer(m_VertexBufferIndex,
                    count,
                    ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE,
                    layout.GetStride(),
                    (const void*)(element.Offset + sizeof(float) * count * i));
                GLES3::GL()->VertexAttribDivisor(m_VertexBufferIndex, 1);
                m_VertexBufferIndex++;
            }
            break;
        }
        default:
            TB_CORE_ASSERT_TAGGED(false, "Unknown ShaderDataType!");
        }
    }

    m_VertexBuffers.push_back(vertexBuffer);
}

void OpenGLES3VertexArray::SetIndexBuffer(const Shared<IndexBuffer>& indexBuffer)
{
    // TB_PROFILE_FUNCTION();

    GLES3::GL()->BindVertexArray(m_RendererID);
    indexBuffer->Bind();

    m_IndexBuffer = indexBuffer;
}

}
