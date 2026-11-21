#pragma once

#include "Forge/Scene/Transform.h"
#include "Forge/Renderer/Mesh.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace Forge {

    class Script;

    struct Entity {
        std::string Name = "Entity";
        MeshType Shape = MeshType::Cube;
        Transform TransformData;
        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };
        bool Visible = true;
        std::string MeshPath;
        std::string TexturePath;

        // Physics
        bool PhysicsEnabled = false;
        bool IsStatic       = false;
        float Bounciness    = 0.35f;
        glm::vec3 Velocity{ 0.0f, 0.0f, 0.0f };

        // Native C++ Scripting
        std::string ScriptName;
        std::shared_ptr<Script> ScriptInstance;
    };

}
