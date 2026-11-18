#pragma once

#include <cstdint>
#include <string>

struct GLFWwindow;

namespace Forge {

    struct WindowProps {
        std::string Title = "ForgeEngine";
        uint32_t Width = 1280;
        uint32_t Height = 720;
    };

    class Window {
    public:
        explicit Window(const WindowProps& props = WindowProps());
        ~Window();

        void OnUpdate();
        bool ShouldClose() const;

        uint32_t GetWidth() const { return m_Data.Width; }
        uint32_t GetHeight() const { return m_Data.Height; }
        GLFWwindow* GetNativeWindow() const { return m_Window; }

    private:
        void Init(const WindowProps& props);
        void Shutdown();

        GLFWwindow* m_Window = nullptr;
        WindowProps m_Data;
    };

}
