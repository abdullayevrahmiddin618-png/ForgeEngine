#pragma once

#include "Forge/Scene/Transform.h"
#include "Forge/Renderer/Mesh.h"
#include <string>
#include <glm/glm.hpp>

namespace Forge {

    struct Entity {
        std::string Name = "Entity";
        MeshType Shape = MeshType::Cube;
        Transform TransformData;
        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };
        bool Visible = true;
    };

}
