#include "Forge/Renderer/Skybox.h"
#include <glad/glad.h>
#include <string>

namespace Forge {

    Skybox::Skybox() {
        const std::string vertexSrc = R"(
            #version 330 core
            uniform mat4 u_View;
            uniform mat4 u_Projection;

            out vec3 v_RayDir;

            vec2 verts[6] = vec2[](
                vec2(-1.0, -1.0), vec2( 1.0, -1.0), vec2( 1.0,  1.0),
                vec2(-1.0, -1.0), vec2( 1.0,  1.0), vec2(-1.0,  1.0)
            );

            void main() {
                vec2 p = verts[gl_VertexID];
                vec4 clip = vec4(p, 1.0, 1.0);
                vec4 eye = inverse(u_Projection) * clip;
                eye = vec4(eye.xy, -1.0, 0.0);
                v_RayDir = (inverse(mat4(mat3(u_View))) * eye).xyz;
                gl_Position = vec4(p, 1.0, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_RayDir;
            uniform vec3 u_SunDir;

            out vec4 FragColor;

            void main() {
                vec3 dir = normalize(v_RayDir);
                vec3 sun = normalize(u_SunDir);

                float h = clamp(dir.y, -1.0, 1.0);

                // Sky & ground gradient
                vec3 zenithColor  = vec3(0.08, 0.18, 0.38);
                vec3 horizonColor = vec3(0.45, 0.62, 0.80);
                vec3 groundColor  = vec3(0.10, 0.11, 0.13);

                vec3 sky = (h >= 0.0)
                    ? mix(horizonColor, zenithColor, pow(h, 0.55))
                    : mix(horizonColor * 0.45, groundColor, clamp(-h * 4.0, 0.0, 1.0));

                // Sun glow + disk
                float sunAmt = max(dot(dir, sun), 0.0);
                sky += vec3(1.0, 0.85, 0.60) * pow(sunAmt, 16.0) * 0.25;
                sky += vec3(1.0, 0.96, 0.88) * smoothstep(0.9985, 0.9996, sunAmt) * 1.4;

                FragColor = vec4(sky, 1.0);
            }
        )";

        m_Shader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
        glGenVertexArrays(1, &m_VAO);
    }

    Skybox::~Skybox() {
        glDeleteVertexArrays(1, &m_VAO);
    }

    void Skybox::Draw(const Camera& camera, const glm::vec3& sunDir) const {
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);

        m_Shader->Bind();
        m_Shader->SetMat4("u_View", camera.GetViewMatrix());
        m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_Shader->SetVec3("u_SunDir", sunDir);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

}
