#pragma once

#include "Forge/Renderer/VertexArray.h"
#include <memory>

namespace Forge {

    enum class MeshType {
        Cube = 0,
        Sphere,
        Pyramid,
        Plane
    };

    class Mesh {
    public:
        static std::unique_ptr<VertexArray> CreateCube();
        static std::unique_ptr<VertexArray> CreatePlane();
        static std::unique_ptr<VertexArray> CreatePyramid();
        static std::unique_ptr<VertexArray> CreateSphere(int sectors = 24, int stacks = 16);
    };

}
