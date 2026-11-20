#include "Forge/Renderer/Grid.h"
#include <glad/glad.h>
#include <string>

namespace Forge {

    Grid::Grid() {
        const std::string vertexSrc = R"(
            #version 330 core
            uniform mat4 u_View;
            uniform mat4 u_Projection;

            out vec3 v_NearPoint;
            out vec3 v_FarPoint;

            vec3 gridPlane[6] = vec3[](
                vec3( 1.0,  1.0, 0.0), vec3(-1.0, -1.0, 0.0), vec3(-1.0,  1.0, 0.0),
                vec3(-1.0, -1.0, 0.0), vec3( 1.0,  1.0, 0.0), vec3( 1.0, -1.0, 0.0)
            );

            vec3 UnprojectPoint(float x, float y, float z, mat4 viewInv, mat4 projInv) {
                vec4 p = viewInv * projInv * vec4(x, y, z, 1.0);
                return p.xyz / p.w;
            }

            void main() {
                vec3 p = gridPlane[gl_VertexID];
                mat4 viewInv = inverse(u_View);
                mat4 projInv = inverse(u_Projection);
                v_NearPoint = UnprojectPoint(p.x, p.y, -1.0, viewInv, projInv);
                v_FarPoint  = UnprojectPoint(p.x, p.y,  1.0, viewInv, projInv);
                gl_Position = vec4(p, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_NearPoint;
            in vec3 v_FarPoint;

            uniform mat4 u_View;
            uniform mat4 u_Projection;
            uniform float u_Near;
            uniform float u_Far;

            out vec4 FragColor;

            vec4 GridColor(vec3 pos, float scale) {
                vec2 coord = pos.xz * scale;
                vec2 deriv = fwidth(coord);
                vec2 grid  = abs(fract(coord - 0.5) - 0.5) / deriv;
                float line = min(grid.x, grid.y);
                float minz = min(deriv.y, 1.0);
                float minx = min(deriv.x, 1.0);

                vec4 color = vec4(0.32, 0.36, 0.42, 1.0 - min(line, 1.0));
                if (pos.x > -0.1 * minx && pos.x < 0.1 * minx)
                    color.rgb = vec3(0.20, 0.55, 0.95);
                if (pos.z > -0.1 * minz && pos.z < 0.1 * minz)
                    color.rgb = vec3(0.95, 0.25, 0.25);
                return color;
            }

            float ComputeDepth(vec3 pos) {
                vec4 clip = u_Projection * u_View * vec4(pos, 1.0);
                return (clip.z / clip.w) * 0.5 + 0.5;
            }

            float ComputeLinearDepth(vec3 pos) {
                vec4 clip = u_Projection * u_View * vec4(pos, 1.0);
                float ndc = clip.z / clip.w;
                float lin = (2.0 * u_Near * u_Far) / (u_Far + u_Near - ndc * (u_Far - u_Near));
                return lin / u_Far;
            }

            void main() {
                float t = -v_NearPoint.y / (v_FarPoint.y - v_NearPoint.y);
                if (t <= 0.0) discard;

                vec3 pos = v_NearPoint + t * (v_FarPoint - v_NearPoint);
                gl_FragDepth = ComputeDepth(pos);

                float fading = max(0.0, 0.5 - ComputeLinearDepth(pos));
                vec4 color = GridColor(pos, 1.0) + GridColor(pos, 0.1) * 0.5;
                color.a *= fading * 2.0;
                if (color.a <= 0.01) discard;
                FragColor = color;
            }
        )";

        m_Shader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
        glGenVertexArrays(1, &m_VAO);
    }

    Grid::~Grid() {
        glDeleteVertexArrays(1, &m_VAO);
    }

    void Grid::Draw(const Camera& camera) const {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_Shader->Bind();
        m_Shader->SetMat4("u_View", camera.GetViewMatrix());
        m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_Shader->SetFloat("u_Near", 0.1f);
        m_Shader->SetFloat("u_Far", 100.0f);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glDisable(GL_BLEND);
    }

}
