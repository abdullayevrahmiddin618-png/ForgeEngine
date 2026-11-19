#include "Forge/Renderer/VertexArray.h"

#include <glad/glad.h>

namespace Forge {

    VertexArray::VertexArray(const std::vector<Vertex>& vertices)
        : m_VertexCount(static_cast<uint32_t>(vertices.size())) {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
            vertices.data(),
            GL_STATIC_DRAW
        );

        // 0: Pozitsiya (x, y, z)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

        // 1: Rang (r, g, b)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(3 * sizeof(float)));

        // 2: Normal (nx, ny, nz)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(6 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    VertexArray::~VertexArray() {
        if (m_VBO) glDeleteBuffers(1, &m_VBO);
        if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    }

    void VertexArray::Bind() const {
        glBindVertexArray(m_VAO);
    }

    void VertexArray::Unbind() const {
        glBindVertexArray(0);
    }

}
