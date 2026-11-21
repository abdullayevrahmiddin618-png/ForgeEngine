#include "Forge/Renderer/SceneRenderer.h"
#include "Forge/Renderer/Renderer.h"

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

            out vec3 v_Normal;
            out vec3 v_FragPos;
            out vec2 v_TexCoord;

            void main() {
                v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                v_TexCoord = a_TexCoord;
                gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_Normal;
            in vec3 v_FragPos;
            in vec2 v_TexCoord;

            uniform vec3 u_ObjectColor;
            uniform vec3 u_LightPos;
            uniform vec3 u_LightColor;
            uniform vec3 u_ViewPos;
            uniform float u_AmbientStrength;
            uniform float u_SpecularStrength;
            uniform sampler2D u_Texture;
            uniform int u_UseTexture;

            out vec4 FragColor;

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

                vec3 baseColor = u_ObjectColor;
                if (u_UseTexture == 1) {
                    baseColor *= texture(u_Texture, v_TexCoord).rgb;
                }
                FragColor = vec4((ambient + diffuse + specular) * baseColor, 1.0);
            }
        )";

        m_Shader         = std::make_unique<Shader>(vertexSrc, fragmentSrc);
        m_DefaultTexture = Texture::CreateCheckerboard(256, 32);
        m_CubeMesh       = Mesh::CreateCube();
        m_SphereMesh     = Mesh::CreateSphere(32, 20);
        m_PyramidMesh    = Mesh::CreatePyramid();
        m_PlaneMesh      = Mesh::CreatePlane();
        m_Grid           = std::make_unique<Grid>();
    }

    SceneRenderer::~SceneRenderer() {
        m_CustomTextures.clear();
        m_CustomMeshes.clear();
        m_Grid.reset();
        m_PlaneMesh.reset();
        m_PyramidMesh.reset();
        m_SphereMesh.reset();
        m_CubeMesh.reset();
        m_DefaultTexture.reset();
        m_Shader.reset();
    }

    void SceneRenderer::Render(const Scene& scene, const Camera& camera, const RenderSettings& settings, bool showGrid) {
        Renderer::SetClearColor(settings.ClearColor.x, settings.ClearColor.y, settings.ClearColor.z, 1.0f);
        Renderer::Clear();

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
        m_Shader->SetVec3("u_LightPos", settings.LightPos);
        m_Shader->SetVec3("u_LightColor", settings.LightColor);
        m_Shader->SetVec3("u_ViewPos", camera.GetPosition());
        m_Shader->SetFloat("u_AmbientStrength", settings.AmbientStrength);
        m_Shader->SetFloat("u_SpecularStrength", settings.SpecularStrength);
        m_Shader->SetInt("u_Texture", 0);

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
