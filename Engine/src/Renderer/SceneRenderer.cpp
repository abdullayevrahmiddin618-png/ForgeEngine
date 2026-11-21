#include "Forge/Renderer/SceneRenderer.h"
#include "Forge/Renderer/Renderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Forge {

    SceneRenderer::SceneRenderer() {
        const std::string vertexSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Color;
            layout (location = 2) in vec3 a_Normal;
            layout (location = 3) in vec2 a_TexCoord;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;
            uniform mat4 u_LightSpaceMatrix;

            out vec3 v_Normal;
            out vec3 v_FragPos;
            out vec2 v_TexCoord;
            out vec4 v_FragPosLightSpace;

            void main() {
                vec4 worldPos = u_Model * vec4(a_Position, 1.0);
                v_FragPos = worldPos.xyz;
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                v_TexCoord = a_TexCoord;
                v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;
                gl_Position = u_ViewProjection * worldPos;
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_Normal;
            in vec3 v_FragPos;
            in vec2 v_TexCoord;
            in vec4 v_FragPosLightSpace;

            uniform vec3 u_ObjectColor;
            uniform vec3 u_LightPos;
            uniform vec3 u_LightColor;
            uniform vec3 u_ViewPos;
            uniform float u_AmbientStrength;
            uniform float u_SpecularStrength;
            uniform sampler2D u_Texture;
            uniform sampler2D u_ShadowMap;
            uniform int u_UseTexture;
            uniform int u_EnableShadows;

            out vec4 FragColor;

            float CalcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
                vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
                projCoords = projCoords * 0.5 + 0.5;
                if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
                    return 0.0;

                float bias = max(0.004 * (1.0 - dot(normal, lightDir)), 0.0008);
                float shadow = 0.0;
                vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);

                // 3x3 PCF soft shadows
                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                        shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
                    }
                }
                return shadow / 9.0;
            }

            void main() {
                vec3 ambient = u_AmbientStrength * u_LightColor;
                vec3 norm = normalize(v_Normal);
                vec3 lightDir = normalize(u_LightPos - v_FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * u_LightColor;

                vec3 viewDir = normalize(u_ViewPos - v_FragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                vec3 specular = u_SpecularStrength * spec * u_LightColor;

                float shadow = (u_EnableShadows == 1) ? CalcShadow(v_FragPosLightSpace, norm, lightDir) : 0.0;

                vec3 baseColor = u_ObjectColor;
                if (u_UseTexture == 1) {
                    baseColor *= texture(u_Texture, v_TexCoord).rgb;
                }

                vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
                FragColor = vec4(lighting * baseColor, 1.0);
            }
        )";

        const std::string shadowVertSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            uniform mat4 u_LightSpaceMatrix;
            uniform mat4 u_Model;
            void main() {
                gl_Position = u_LightSpaceMatrix * u_Model * vec4(a_Position, 1.0);
            }
        )";

        const std::string shadowFragSrc = R"(
            #version 330 core
            void main() {}
        )";

        m_Shader         = std::make_unique<Shader>(vertexSrc, fragmentSrc);
        m_ShadowShader   = std::make_unique<Shader>(shadowVertSrc, shadowFragSrc);
        m_DefaultTexture = Texture::CreateCheckerboard(256, 32);
        m_CubeMesh       = Mesh::CreateCube();
        m_SphereMesh     = Mesh::CreateSphere(32, 20);
        m_PyramidMesh    = Mesh::CreatePyramid();
        m_PlaneMesh      = Mesh::CreatePlane();
        m_Grid           = std::make_unique<Grid>();
        m_Skybox         = std::make_unique<Skybox>();

        InitShadowMap();
    }

    SceneRenderer::~SceneRenderer() {
        if (m_ShadowFBO) glDeleteFramebuffers(1, &m_ShadowFBO);
        if (m_ShadowMap) glDeleteTextures(1, &m_ShadowMap);

        m_CustomTextures.clear();
        m_CustomMeshes.clear();
        m_Skybox.reset();
        m_Grid.reset();
        m_PlaneMesh.reset();
        m_PyramidMesh.reset();
        m_SphereMesh.reset();
        m_CubeMesh.reset();
        m_DefaultTexture.reset();
        m_ShadowShader.reset();
        m_Shader.reset();
    }

    void SceneRenderer::InitShadowMap() {
        glGenFramebuffers(1, &m_ShadowFBO);

        glGenTextures(1, &m_ShadowMap);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowMapSize, m_ShadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void SceneRenderer::Render(const Scene& scene, const Camera& camera, const RenderSettings& settings, bool showGrid) {
        // Compute light-space matrix for directional shadows
        glm::mat4 lightProj = glm::ortho(-14.0f, 14.0f, -14.0f, 14.0f, 0.5f, 35.0f);
        glm::vec3 lightDir  = glm::length(settings.LightPos) > 0.01f ? settings.LightPos : glm::vec3(4.0f, 7.0f, 4.0f);
        glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProj * lightView;

        // 1. Shadow Depth Pass (preserve current Viewport FBO & Viewport size)
        if (settings.EnableShadows) {
            GLint prevFBO = 0;
            GLint prevViewport[4] = { 0, 0, 0, 0 };
            glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
            glGetIntegerv(GL_VIEWPORT, prevViewport);

            glViewport(0, 0, m_ShadowMapSize, m_ShadowMapSize);
            glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowFBO);
            glClear(GL_DEPTH_BUFFER_BIT);

            m_ShadowShader->Bind();
            m_ShadowShader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);

            for (const auto& entity : scene.GetEntities()) {
                if (!entity.Visible) continue;
                m_ShadowShader->SetMat4("u_Model", entity.TransformData.GetModelMatrix());
                Renderer::Draw(GetEntityMesh(entity), *m_ShadowShader);
            }

            glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(prevFBO));
            glViewport(prevViewport[0], prevViewport[1], prevViewport[2], prevViewport[3]);
        }

        // 2. Main Scene Pass
        Renderer::SetClearColor(settings.ClearColor.x, settings.ClearColor.y, settings.ClearColor.z, 1.0f);
        Renderer::Clear();

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
        m_Shader->SetMat4("u_LightSpaceMatrix", lightSpaceMatrix);
        m_Shader->SetVec3("u_LightPos", settings.LightPos);
        m_Shader->SetVec3("u_LightColor", settings.LightColor);
        m_Shader->SetVec3("u_ViewPos", camera.GetPosition());
        m_Shader->SetFloat("u_AmbientStrength", settings.AmbientStrength);
        m_Shader->SetFloat("u_SpecularStrength", settings.SpecularStrength);
        m_Shader->SetInt("u_Texture", 0);
        m_Shader->SetInt("u_ShadowMap", 1);
        m_Shader->SetInt("u_EnableShadows", settings.EnableShadows ? 1 : 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_ShadowMap);

        for (const auto& entity : scene.GetEntities()) {
            if (!entity.Visible) continue;

            const Texture* customTex = GetEntityTexture(entity);
            if (customTex) {
                customTex->Bind(0);
                m_Shader->SetInt("u_UseTexture", 1);
            } else {
                m_DefaultTexture->Bind(0);
                m_Shader->SetInt("u_UseTexture", settings.UseTexture ? 1 : 0);
            }

            m_Shader->SetMat4("u_Model", entity.TransformData.GetModelMatrix());
            m_Shader->SetVec3("u_ObjectColor", entity.Color);
            Renderer::Draw(GetEntityMesh(entity), *m_Shader);
        }

        // 3. Skybox & Infinite 3D Grid
        if (settings.EnableSkybox) {
            m_Skybox->Draw(camera, lightDir);
        }

        if (showGrid) {
            m_Grid->Draw(camera);
        }
    }

    const VertexArray& SceneRenderer::GetEntityMesh(const Entity& entity) {
        if (entity.Shape == MeshType::Custom && !entity.MeshPath.empty()) {
            auto it = m_CustomMeshes.find(entity.MeshPath);
            if (it == m_CustomMeshes.end()) {
                auto loaded = Mesh::LoadOBJ(entity.MeshPath);
                it = m_CustomMeshes.emplace(entity.MeshPath, std::move(loaded)).first;
            }
            if (it->second) {
                return *it->second;
            }
        }

        switch (entity.Shape) {
            case MeshType::Sphere:  return *m_SphereMesh;
            case MeshType::Pyramid: return *m_PyramidMesh;
            case MeshType::Plane:   return *m_PlaneMesh;
            case MeshType::Cube:
            default:                return *m_CubeMesh;
        }
    }

    const Texture* SceneRenderer::GetEntityTexture(const Entity& entity) {
        if (entity.TexturePath.empty()) return nullptr;

        auto it = m_CustomTextures.find(entity.TexturePath);
        if (it == m_CustomTextures.end()) {
            auto tex = std::make_unique<Texture>(entity.TexturePath);
            if (tex->GetWidth() == 0) {
                tex.reset();
            }
            it = m_CustomTextures.emplace(entity.TexturePath, std::move(tex)).first;
        }
        return it->second.get();
    }

}
