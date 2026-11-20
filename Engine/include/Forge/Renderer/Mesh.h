#pragma once

#include "Forge/Renderer/VertexArray.h"
#include <memory>
#include <string>

namespace Forge {

    enum class MeshType {
        Cube = 0,
        Sphere,
        Pyramid,
        Plane,
        Custom
    };

    class Mesh {
    public:
        static std::unique_ptr<VertexArray> CreateCube();
        static std::unique_ptr<VertexArray> CreatePlane();
        static std::unique_ptr<VertexArray> CreatePyramid();
        static std::unique_ptr<VertexArray> CreateSphere(int sectors = 24, int stacks = 16);
        static std::unique_ptr<VertexArray> LoadOBJ(const std::string& filepath);
    };

}
