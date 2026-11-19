#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>

namespace Forge {

    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        Shader(const Shader&) = delete;
        Shader& operator=(const Shader&) = delete;

        void Bind() const;
        void Unbind() const;

        void SetFloat(const std::string& name, float value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& matrix) const;

    private:
        uint32_t Compile(uint32_t type, const std::string& source);

        uint32_t m_RendererID = 0;
    };

}
