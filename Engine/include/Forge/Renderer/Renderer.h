#pragma once

namespace Forge {

    class Renderer {
    public:
        static void SetClearColor(float r, float g, float b, float a = 1.0f);
        static void Clear();
    };

}
