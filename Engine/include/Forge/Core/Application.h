#pragma once

namespace Forge {

    class Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Run();

    protected:
        virtual void OnInit() {}
        virtual void OnUpdate() {}
        virtual void OnShutdown() {}

        bool m_Running = true;
    };

}
