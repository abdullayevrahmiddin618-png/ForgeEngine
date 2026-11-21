#pragma once
#include "Forge/Renderer/Camera.h"
#include "Forge/Renderer/Grid.h"
#include "Forge/Renderer/Mesh.h"
#include "Forge/Renderer/Shader.h"
#include "Forge/Renderer/Texture.h"
#include "Forge/Renderer/VertexArray.h"
#include "Forge/Scene/Scene.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace Forge {

    struct RenderSettings {
        glm::vec3 ClearColor{ 0.07f, 0.08f, 0.11f };
        glm::vec3 LightPos{ 2.5f, 4.0f, 2.5f };
        glm::vec3 LightColor{ 1.0f, 1.0f, 1.0f };
        float AmbientStrength  = 0.22f;
        float SpecularStrength = 0.55f;
        bool UseTexture        = true;
    };

    class SceneRenderer {
    public:
        SceneRenderer();
        ~SceneRenderer();

        void Render(const Scene& scene, const Camera& camera, const RenderSettings& settings, bool showGrid);

    private:
        const VertexArray& GetEntityMesh(const Entity& entity);
        const Texture* GetEntityTexture(const Entity& entity);

        std::unique_ptr<Shader> m_Shader;
        std::unique_ptr<Texture> m_DefaultTexture;
        std::unique_ptr<VertexArray> m_CubeMesh;
        std::unique_ptr<VertexArray> m_SphereMesh;
        std::unique_ptr<VertexArray> m_PyramidMesh;
        std::unique_ptr<VertexArray> m_PlaneMesh;
        std::unique_ptr<Grid> m_Grid;

        std::unordered_map<std::string, std::unique_ptr<VertexArray>> m_CustomMeshes;
        std::unordered_map<std::string, std::unique_ptr<Texture>> m_CustomTextures;
    };

}
