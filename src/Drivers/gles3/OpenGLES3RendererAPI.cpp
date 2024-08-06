#include "Drivers/gles3/OpenGLES3RendererAPI.h"
#include "Drivers/gles3/GLES3.h"
#include "tbpch.h"

#include <gles3.h>

namespace Tabby {

// void OpenGLMessageCallback(
//     unsigned source,
//     unsigned type,
//     unsigned id,
//     unsigned severity,
//     int length,
//     const char* message,
//     const void* userParam)
// {
//     switch (severity) {
//         // case GL_DEBUG_SEVERITY_HIGH:         TB_CORE_CRITICAL(message); return;
//         // case GL_DEBUG_SEVERITY_MEDIUM:       TB_CORE_ERROR(message); return;
//         // case GL_DEBUG_SEVERITY_LOW:          TB_CORE_WARN(message); return;
//         // case GL_DEBUG_SEVERITY_NOTIFICATION: TB_CORE_TRACE(message); return;
//     }
//
//     TB_CORE_ASSERT(false, "Unknown severity level!");
// }

void OpenGLES3RendererAPI::Init()
{
    // TB_PROFILE_FUNCTION();

#ifdef TB_DEBUG
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(OpenGLMessageCallback, nullptr);
    //
    // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

    GLES3::GL()->Enable(GL_BLEND);
    GLES3::GL()->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLES3::GL()->Enable(GL_DEPTH_TEST);
    // glEnable(GL_LINE_SMOOTH);
}

void OpenGLES3RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    GLES3::GL()->Viewport(x, y, width, height);
}

void OpenGLES3RendererAPI::SetClearColor(const glm::vec4& color)
{
    GLES3::GL()->ClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLES3RendererAPI::Clear()
{
    GLES3::GL()->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLES3RendererAPI::DrawIndexed(const Shared<VertexArray>& vertexArray, uint32_t indexCount)
{
    vertexArray->Bind();
    uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
    GLES3::GL()->DrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLES3RendererAPI::DrawLines(const Shared<VertexArray>& vertexArray, uint32_t vertexCount)
{
    vertexArray->Bind();
    GLES3::GL()->DrawArrays(GL_LINES, 0, vertexCount);
}

void OpenGLES3RendererAPI::SetLineWidth(float width)
{
    GLES3::GL()->LineWidth(width);
}

void OpenGLES3RendererAPI::EnableDepthTest(bool enable)
{
    if (enable) {
        GLES3::GL()->Enable(GL_DEPTH_TEST);
    } else {
        GLES3::GL()->Disable(GL_DEPTH_TEST);
    }
}

void OpenGLES3RendererAPI::EnableAlphaBlending(bool enable)
{
    if (enable) {
        GLES3::GL()->Enable(GL_BLEND);
        GLES3::GL()->BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        GLES3::GL()->Disable(GL_BLEND);
    }
}

}
