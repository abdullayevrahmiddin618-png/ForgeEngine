
#include "Engine.h"
#include "Logger.h"

namespace Forge
{
    Engine::Engine()
        : m_Running(false),
          m_Initialized(false)
    {
    }

    Engine::~Engine()
    {
        Shutdown();
    }

    bool Engine::Initialize()
    {
        if (m_Initialized)
            return true;

        Logger::Info("ForgeEngine is initializing...");

        m_Running = true;
        m_Initialized = true;

        Logger::Info("ForgeEngine initialized successfully!");

        return true;
    }

    void Engine::Run()
    {
        if (!m_Initialized)
        {
            Logger::Error("Engine is not initialized!");
            return;
        }

        Logger::Info("ForgeEngine main loop started.");

        while (m_Running)
        {
            // Engine update system will be implemented here.
            m_Running = false;
        }

        Logger::Info("ForgeEngine main loop stopped.");
    }

    void Engine::Shutdown()
    {
        if (!m_Initialized)
            return;

        Logger::Info("Shutting down ForgeEngine...");

        m_Running = false;
        m_Initialized = false;

        Logger::Info("ForgeEngine shutdown complete.");
    }

    bool Engine::IsRunning() const
    {
        return m_Running;
    }
}
