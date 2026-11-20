#pragma once
#include "Forge/Renderer/Camera.h"
#include "Forge/Renderer/Shader.h"
#include <cstdint>
#include <memory>

namespace Forge {

    class Grid {
    public:
        Grid();
        ~Grid();

        void Draw(const Camera& camera) const;

    private:
        std::unique_ptr<Shader> m_Shader;
        uint32_t m_VAO = 0;
    };

}
