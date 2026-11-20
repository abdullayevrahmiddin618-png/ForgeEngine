#pragma once
#include "Forge/Scene/Scene.h"

namespace Forge {

    class PhysicsWorld {
    public:
        static void Step(Scene& scene, float dt, float gravity = -9.81f);
    };

}
