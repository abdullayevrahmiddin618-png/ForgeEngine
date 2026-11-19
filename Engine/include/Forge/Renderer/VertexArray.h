#pragma once

#include <cstdint>
#include <vector>

namespace Forge {

    struct Vertex {
        float x, y, z;
        float r, g, b;
        float nx, ny, nz;
    };

    class VertexArray {
    public:
        explicit VertexArray(const std::vector<Vertex>& vertices);
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;

        void Bind() const;
        void Unbind() const;

        uint32_t GetVertexCount() const { return m_VertexCount; }

    private:
        uint32_t m_VAO = 0;
        uint32_t m_VBO = 0;
        uint32_t m_VertexCount = 0;
    };

}
