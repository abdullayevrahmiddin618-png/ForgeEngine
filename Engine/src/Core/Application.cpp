#include "Forge/Core/Application.h"
#include "Forge/Core/Log.h"

namespace Forge {

    Application::Application(const WindowProps& props)
        : m_WindowProps(props) {}

    void Application::Run() {
        FE_INFO("ForgeEngine ishga tushdi.");

        m_Window = std::make_unique<Window>(m_WindowProps);
        OnInit();

        while (m_Running && !m_Window->ShouldClose()) {
            OnUpdate();
            m_Window->OnUpdate();
        }

        OnShutdown();
        m_Window.reset();

        FE_INFO("ForgeEngine yopildi.");
    }

}
