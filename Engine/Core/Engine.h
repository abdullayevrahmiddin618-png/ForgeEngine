
#pragma once

#include <string>

namespace Forge
{
    class Engine
    {
    public:
        Engine();
        ~Engine();

        bool Initialize();
        void Run();
        void Shutdown();

        bool IsRunning() const;

    private:
        bool m_Running;
        bool m_Initialized;
    };
}
