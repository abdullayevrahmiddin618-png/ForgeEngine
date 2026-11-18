#pragma once

#include "Forge/Core/Window.h"
#include <memory>

namespace Forge {

    class Application {
    public:
        explicit Application(const WindowProps& props = WindowProps());
        virtual ~Application() = default;

        void Run();

    protected:
        virtual void OnInit() {}
        virtual void OnUpdate() {}
        virtual void OnShutdown() {}

        Window& GetWindow() { return *m_Window; }

        bool m_Running = true;

    private:
        WindowProps m_WindowProps;
        std::unique_ptr<Window> m_Window;
    };

}
