#include "Forge/Core/Application.h"
#include "Forge/Core/Log.h"

namespace Forge {

    void Application::Run() {
        FE_INFO("ForgeEngine ishga tushdi.");
        OnInit();

        while (m_Running) {
            OnUpdate();
            m_Running = false; // Hozircha 1 marta aylanib to'xtaydi (oyna qo'shilguncha)
        }

        OnShutdown();
        FE_INFO("ForgeEngine yopildi.");
    }

}
