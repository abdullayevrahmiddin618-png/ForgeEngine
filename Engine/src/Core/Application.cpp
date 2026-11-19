#include "Forge/Core/Application.h"
#include "Forge/Core/Log.h"
#include "Forge/Renderer/Renderer.h"

#include <chrono>

namespace Forge {

    Application* Application::s_Instance = nullptr;

    Application::Application(const WindowProps& props)
        : m_WindowProps(props) {
        s_Instance = this;
    }

    void Application::Run() {
        FE_INFO("ForgeEngine ishga tushdi.");

        m_Window = std::make_unique<Window>(m_WindowProps);
        Renderer::Init();

        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window->GetNativeWindow());

        OnInit();

        using Clock = std::chrono::steady_clock;
        auto lastTime = Clock::now();

        while (m_Running && !m_Window->ShouldClose()) {
            auto currentTime = Clock::now();
            float dt = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;

            OnUpdate(dt);

            m_ImGuiLayer->Begin();
            OnImGuiRender();
            m_ImGuiLayer->End();

            m_Window->OnUpdate();
        }

        OnShutdown();
        m_ImGuiLayer->Shutdown();
        m_ImGuiLayer.reset();
        m_Window.reset();

        FE_INFO("ForgeEngine yopildi.");
    }

}
