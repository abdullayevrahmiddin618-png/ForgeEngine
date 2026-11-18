#pragma once

#include <cstdint>
#include <string>

namespace Forge {

    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void Bind() const;
        void Unbind() const;

    private:
        uint32_t Compile(uint32_t type, const std::string& source);

        uint32_t m_RendererID = 0;
    };

}
