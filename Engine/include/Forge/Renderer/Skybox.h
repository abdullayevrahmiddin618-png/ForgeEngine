#pragma once
#include "Forge/Renderer/Camera.h"
#include "Forge/Renderer/Shader.h"
#include <glm/glm.hpp>
#include <cstdint>
#include <memory>

namespace Forge {

    class Skybox {
    public:
        Skybox();
        ~Skybox();

        void Draw(const Camera& camera, const glm::vec3& sunDir) const;

    private:
        std::unique_ptr<Shader> m_Shader;
        uint32_t m_VAO = 0;
    };

}
