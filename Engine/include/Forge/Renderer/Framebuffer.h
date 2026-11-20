#pragma once
#include <cstdint>

namespace Forge {

    class Framebuffer {
    public:
        Framebuffer(uint32_t width, uint32_t height);
        ~Framebuffer();

        void Bind() const;
        void Unbind() const;
        void Resize(uint32_t width, uint32_t height);

        uint32_t GetColorAttachmentID() const { return m_ColorAttachment; }
        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

    private:
        void Invalidate();

        uint32_t m_RendererID      = 0;
        uint32_t m_ColorAttachment = 0;
        uint32_t m_DepthAttachment = 0;
        uint32_t m_Width           = 1280;
        uint32_t m_Height          = 720;
    };

}
