#pragma once

#include "Tabby/Renderer/Framebuffer.h"

namespace Tabby {

class OpenGLES3Framebuffer : public Framebuffer {
public:
    OpenGLES3Framebuffer(const FramebufferSpecification& spec);
    virtual ~OpenGLES3Framebuffer();

    void Invalidate();

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;

    virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

    virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override
    {
        // TB_CORE_ASSERT(index < m_ColorAttachments.size());
        return m_ColorAttachment;
    }

    virtual const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

private:
    uint32_t m_RendererID = 0;
    FramebufferSpecification m_Specification;

    std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
    FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

    // std::vector<uint32_t> m_ColorAttachments;
    uint32_t m_ColorAttachment = 0;
    uint32_t m_DepthAttachment = 0;
};

}
