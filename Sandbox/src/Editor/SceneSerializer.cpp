#include "SceneSerializer.h"
#include <fstream>
#include <iomanip>
#include <sstream>

bool SceneSerializer::Save(const std::string& filepath, Forge::Scene& scene, const WorldSettings& world) {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    out << "FORGE_SCENE_V1\n";
    out << "WORLD "
        << world.ClearColor.x << " " << world.ClearColor.y << " " << world.ClearColor.z << " "
        << world.LightPos.x << " " << world.LightPos.y << " " << world.LightPos.z << " "
        << world.LightColor.x << " " << world.LightColor.y << " " << world.LightColor.z << " "
        << world.AmbientStrength << " " << world.SpecularStrength << " "
        << (world.UseTexture ? 1 : 0) << "\n";

    for (const auto& e : scene.GetEntities()) {
        out << "ENTITY "
            << std::quoted(e.Name) << " "
            << static_cast<int>(e.Shape) << " "
            << (e.Visible ? 1 : 0) << " "
            << e.TransformData.Position.x << " " << e.TransformData.Position.y << " " << e.TransformData.Position.z << " "
            << e.TransformData.Rotation.x << " " << e.TransformData.Rotation.y << " " << e.TransformData.Rotation.z << " "
            << e.TransformData.Scale.x << " " << e.TransformData.Scale.y << " " << e.TransformData.Scale.z << " "
            << e.Color.x << " " << e.Color.y << " " << e.Color.z << " "
            << std::quoted(e.MeshPath) << " "
            << std::quoted(e.TexturePath) << " "
            << (e.PhysicsEnabled ? 1 : 0) << " "
            << (e.IsStatic ? 1 : 0) << " "
            << e.Bounciness << "\n";
    }

    return true;
}

bool SceneSerializer::Load(const std::string& filepath, Forge::Scene& scene, WorldSettings& world) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::string header;
    std::getline(in, header);
    if (header != "FORGE_SCENE_V1") return false;

    while (!scene.GetEntities().empty()) {
        scene.RemoveEntity(0);
    }

    std::string line;
    while (std::getline(in, line)) {
        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "WORLD") {
            int useTex = 1;
            iss >> world.ClearColor.x >> world.ClearColor.y >> world.ClearColor.z
                >> world.LightPos.x >> world.LightPos.y >> world.LightPos.z
                >> world.LightColor.x >> world.LightColor.y >> world.LightColor.z
                >> world.AmbientStrength >> world.SpecularStrength >> useTex;
            world.UseTexture = (useTex != 0);
        } else if (tag == "ENTITY") {
            std::string name, meshPath, texPath;
            int shapeInt = 0, visInt = 1;
            int physInt = 0, staticInt = 0;
            float bounce = 0.35f;
            glm::vec3 pos, rot, scale, color;

            iss >> std::quoted(name) >> shapeInt >> visInt
                >> pos.x >> pos.y >> pos.z
                >> rot.x >> rot.y >> rot.z
                >> scale.x >> scale.y >> scale.z
                >> color.x >> color.y >> color.z;

            if (iss >> std::quoted(meshPath)) {
                iss >> std::quoted(texPath);
                if (iss >> physInt >> staticInt >> bounce) {
                    // Fizika ma'lumotlari o'qildi
                }
            }

            auto& e = scene.CreateEntity(name);
            e.Shape                  = static_cast<Forge::MeshType>(shapeInt);
            e.Visible                = (visInt != 0);
            e.TransformData.Position = pos;
            e.TransformData.Rotation = rot;
            e.TransformData.Scale    = scale;
            e.Color                  = color;
            e.MeshPath               = meshPath;
            e.TexturePath            = texPath;
            e.PhysicsEnabled         = (physInt != 0);
            e.IsStatic               = (staticInt != 0);
            e.Bounciness             = bounce;
            e.Velocity               = { 0.0f, 0.0f, 0.0f };
        }
    }

    return true;
}
