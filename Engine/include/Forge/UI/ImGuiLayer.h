#pragma once

struct GLFWwindow;

namespace Forge {

    class ImGuiLayer {
    public:
        void Init(GLFWwindow* window);
        void Shutdown();

        void Begin();
        void End();

    private:
        bool m_Initialized = false;
    };

}
