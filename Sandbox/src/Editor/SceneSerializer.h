#pragma once
#include <Forge/Forge.h>
#include <string>

using WorldSettings = Forge::RenderSettings;

class SceneSerializer {
public:
    static bool Save(const std::string& filepath, Forge::Scene& scene, const WorldSettings& world);
    static bool Load(const std::string& filepath, Forge::Scene& scene, WorldSettings& world);
};
