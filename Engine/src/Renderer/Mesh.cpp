#include "Forge/Renderer/Mesh.h"

#include <cmath>
#include <vector>

namespace Forge {

    namespace {
        constexpr float PI = 3.14159265359f;
    }

    std::unique_ptr<VertexArray> Mesh::CreateCube() {
        return std::make_unique<VertexArray>(std::vector<Vertex>{
            // Old (0, 0, 1)
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f,  1.0f, 1.0f },
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f,  0.0f, 1.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f,  0.0f, 0.0f },

            // Orqa (0, 0, -1)
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f,  1.0f, 0.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f,  1.0f, 1.0f },
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f,  0.0f, 1.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f,  0.0f, 0.0f },

            // Chap (-1, 0, 0)
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f },
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f,  0.0f, 1.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f,  0.0f, 0.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f,  1.0f, 0.0f },
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f,  1.0f, 1.0f },

            // O'ng (1, 0, 0)
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f,  1.0f, 1.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f,  1.0f, 0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f,  0.0f, 0.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f },

            // Tepa (0, 1, 0)
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f,  1.0f, 1.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f,  1.0f, 0.0f },
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f },
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f,  0.0f, 1.0f },

            // Past (0, -1, 0)
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f }
        });
    }

    std::unique_ptr<VertexArray> Mesh::CreatePlane() {
        return std::make_unique<VertexArray>(std::vector<Vertex>{
            { -0.5f, 0.0f, -0.5f,  1, 1, 1,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f },
            {  0.5f, 0.0f, -0.5f,  1, 1, 1,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f },
            {  0.5f, 0.0f,  0.5f,  1, 1, 1,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f },
            {  0.5f, 0.0f,  0.5f,  1, 1, 1,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f },
            { -0.5f, 0.0f,  0.5f,  1, 1, 1,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f },
            { -0.5f, 0.0f, -0.5f,  1, 1, 1,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f }
        });
    }

    std::unique_ptr<VertexArray> Mesh::CreatePyramid() {
        return std::make_unique<VertexArray>(std::vector<Vertex>{
            // Asos (past)
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  1.0f, 0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  1.0f, 1.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  0.0f, 1.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f,  0.0f, 0.0f },

            // Old tomon
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.45f,  0.89f,  0.0f, 0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.45f,  0.89f,  1.0f, 0.0f },
            {  0.0f,  0.5f,  0.0f,  1, 1, 1,   0.0f,  0.45f,  0.89f,  0.5f, 1.0f },

            // O'ng tomon
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.89f, 0.45f,  0.0f,  0.0f, 0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.89f, 0.45f,  0.0f,  1.0f, 0.0f },
            {  0.0f,  0.5f,  0.0f,  1, 1, 1,   0.89f, 0.45f,  0.0f,  0.5f, 1.0f },

            // Orqa tomon
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.45f, -0.89f,  0.0f, 0.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.45f, -0.89f,  1.0f, 0.0f },
            {  0.0f,  0.5f,  0.0f,  1, 1, 1,   0.0f,  0.45f, -0.89f,  0.5f, 1.0f },

            // Chap tomon
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,  -0.89f, 0.45f,  0.0f,  0.0f, 0.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,  -0.89f, 0.45f,  0.0f,  1.0f, 0.0f },
            {  0.0f,  0.5f,  0.0f,  1, 1, 1,  -0.89f, 0.45f,  0.0f,  0.5f, 1.0f }
        });
    }

    std::unique_ptr<VertexArray> Mesh::CreateSphere(int sectors, int stacks) {
        std::vector<Vertex> vertices;
        const float radius = 0.5f;

        auto makeVertex = [&](int i, int j) -> Vertex {
            float stackAngle = PI / 2.0f - static_cast<float>(i) * (PI / static_cast<float>(stacks));
            float sectorAngle = static_cast<float>(j) * (2.0f * PI / static_cast<float>(sectors));

            float nx = std::cos(stackAngle) * std::cos(sectorAngle);
            float ny = std::sin(stackAngle);
            float nz = std::cos(stackAngle) * std::sin(sectorAngle);

            float u = static_cast<float>(j) / static_cast<float>(sectors);
            float v = 1.0f - static_cast<float>(i) / static_cast<float>(stacks);

            return Vertex{
                nx * radius, ny * radius, nz * radius,
                1.0f, 1.0f, 1.0f,
                nx, ny, nz,
                u, v
            };
        };

        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < sectors; ++j) {
                Vertex v1 = makeVertex(i, j);
                Vertex v2 = makeVertex(i + 1, j);
                Vertex v3 = makeVertex(i + 1, j + 1);
                Vertex v4 = makeVertex(i, j + 1);

                vertices.push_back(v1);
                vertices.push_back(v2);
                vertices.push_back(v3);

                vertices.push_back(v1);
                vertices.push_back(v3);
                vertices.push_back(v4);
            }
        }

        return std::make_unique<VertexArray>(vertices);
    }

}
