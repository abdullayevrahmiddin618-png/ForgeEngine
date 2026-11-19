#include "Forge/Renderer/Texture.h"
#include "Forge/Core/Log.h"

#include <glad/glad.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Forge {

    Texture::Texture(const std::string& path) {
        stbi_set_flip_vertically_on_load(1);

        int width = 0, height = 0, channels = 0;
        uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) {
            FE_ERROR("Tekstura yuklanmadi: " + path);
            return;
        }

        m_Width = static_cast<uint32_t>(width);
        m_Height = static_cast<uint32_t>(height);
        Upload(data, static_cast<uint32_t>(channels));

        stbi_image_free(data);
    }

    Texture::Texture(uint32_t width, uint32_t height, const uint8_t* rgbaData)
        : m_Width(width), m_Height(height) {
        Upload(rgbaData, 4);
    }

    Texture::~Texture() {
        if (m_RendererID) {
            glDeleteTextures(1, &m_RendererID);
        }
    }

    void Texture::Bind(uint32_t slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

    void Texture::Unbind() const {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    std::unique_ptr<Texture> Texture::CreateCheckerboard(uint32_t size, uint32_t tileSize) {
        std::vector<uint8_t> pixels(size * size * 4);
        for (uint32_t y = 0; y < size; ++y) {
            for (uint32_t x = 0; x < size; ++x) {
                bool isLight = ((x / tileSize) + (y / tileSize)) % 2 == 0;
                uint8_t c = isLight ? 235 : 95;
                size_t idx = (y * size + x) * 4;
                pixels[idx + 0] = c;
                pixels[idx + 1] = c;
                pixels[idx + 2] = c;
                pixels[idx + 3] = 255;
            }
        }
        return std::make_unique<Texture>(size, size, pixels.data());
    }

    void Texture::Upload(const uint8_t* data, uint32_t channels) {
        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        glGenTextures(1, &m_RendererID);
        glBindTexture(GL_TEXTURE_2D, m_RendererID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format),
                     static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height),
                     0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

}
