#include "Forge/Renderer/Shader.h"
#include "Forge/Core/Log.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace Forge {

    Shader::Shader(const std::string& vertexSrc, const std::string& fragmentSrc) {
        uint32_t vs = Compile(GL_VERTEX_SHADER, vertexSrc);
        uint32_t fs = Compile(GL_FRAGMENT_SHADER, fragmentSrc);

        m_RendererID = glCreateProgram();
        glAttachShader(m_RendererID, vs);
        glAttachShader(m_RendererID, fs);
        glLinkProgram(m_RendererID);

        int success = 0;
        glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
        if (!success) {
            int length = 0;
            glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> infoLog(length);
            glGetProgramInfoLog(m_RendererID, length, &length, infoLog.data());
            FE_ERROR(std::string("Shader link xatosi: ") + infoLog.data());
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    Shader::~Shader() {
        if (m_RendererID) {
            glDeleteProgram(m_RendererID);
        }
    }

    void Shader::Bind() const {
        glUseProgram(m_RendererID);
    }

    void Shader::Unbind() const {
        glUseProgram(0);
    }

    void Shader::SetInt(const std::string& name, int value) const {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void Shader::SetFloat(const std::string& name, float value) const {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void Shader::SetMat4(const std::string& name, const glm::mat4& matrix) const {
        int location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    uint32_t Shader::Compile(uint32_t type, const std::string& source) {
        uint32_t id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int success = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            int length = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> infoLog(length);
            glGetShaderInfoLog(id, length, &length, infoLog.data());
            FE_ERROR(std::string("Shader kompilyatsiya xatosi: ") + infoLog.data());
        }

        return id;
    }

}
