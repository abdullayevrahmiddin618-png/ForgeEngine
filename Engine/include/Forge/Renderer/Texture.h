#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace Forge {

    class Texture {
    public:
        explicit Texture(const std::string& path);
        Texture(uint32_t width, uint32_t height, const uint8_t* rgbaData);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        void Bind(uint32_t slot = 0) const;
        void Unbind() const;

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        static std::unique_ptr<Texture> CreateCheckerboard(uint32_t size = 256, uint32_t tileSize = 32);

    private:
        void Upload(const uint8_t* data, uint32_t channels);

        uint32_t m_RendererID = 0;
        uint32_t m_Width = 0;
        uint32_t m_Height = 0;
    };

}
