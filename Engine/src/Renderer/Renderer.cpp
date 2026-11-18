#include "Forge/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Forge {

    void Renderer::Init() {
        glEnable(GL_DEPTH_TEST);
    }

    void Renderer::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void Renderer::Clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::Draw(const VertexArray& vertexArray, const Shader& shader) {
        shader.Bind();
        vertexArray.Bind();
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexArray.GetVertexCount()));
    }

}
