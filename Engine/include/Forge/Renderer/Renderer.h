#pragma once

#include "Forge/Renderer/Shader.h"
#include "Forge/Renderer/VertexArray.h"

namespace Forge {

    class Renderer {
    public:
        static void SetClearColor(float r, float g, float b, float a = 1.0f);
        static void Clear();
        static void Draw(const VertexArray& vertexArray, const Shader& shader);
    };

}
