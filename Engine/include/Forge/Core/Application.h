#pragma once

#include "Forge/Core/Window.h"
#include "Forge/UI/ImGuiLayer.h"
#include <memory>

namespace Forge {

    class Application {
    public:
        explicit Application(const WindowProps& props = WindowProps());
        virtual ~Application() = default;

        void Run();
        void Close() { m_Running = false; }

        Window& GetWindow() { return *m_Window; }
        static Application& Get() { return *s_Instance; }

    protected:
        virtual void OnInit() {}
        virtual void OnUpdate(float dt) { (void)dt; }
        virtual void OnImGuiRender() {}
        virtual void OnShutdown() {}

        bool m_Running = true;

    private:
        WindowProps m_WindowProps;
        std::unique_ptr<Window> m_Window;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
        static Application* s_Instance;
    };

}
