#pragma once
#include <Forge/Forge.h>
#include <glm/glm.hpp>
#include <string>

struct WorldSettings {
    glm::vec3 ClearColor{ 0.07f, 0.08f, 0.11f };
    glm::vec3 LightPos{ 2.5f, 3.5f, 2.5f };
    glm::vec3 LightColor{ 1.0f, 1.0f, 1.0f };
    float AmbientStrength  = 0.22f;
    float SpecularStrength = 0.55f;
    bool UseTexture        = true;
};

class SceneSerializer {
public:
    static bool Save(const std::string& filepath, Forge::Scene& scene, const WorldSettings& world);
    static bool Load(const std::string& filepath, Forge::Scene& scene, WorldSettings& world);
};
