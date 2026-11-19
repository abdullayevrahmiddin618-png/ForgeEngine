#pragma once

#include "Forge/Scene/Transform.h"
#include <string>
#include <glm/glm.hpp>

namespace Forge {

    struct Entity {
        std::string Name = "Entity";
        Transform TransformData;
        glm::vec3 Color{ 0.8f, 0.8f, 0.8f };
        bool Visible = true;
    };

}
