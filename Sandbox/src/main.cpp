#include <Forge/Forge.h>

class SandboxApp : public Forge::Application {
public:
    SandboxApp()
        : Forge::Application({ "ForgeEngine - Sandbox", 1280, 720 }) {}

protected:
    void OnInit() override {
        FE_INFO("Sandbox: OnInit()");
    }

    void OnUpdate() override {
        Forge::Renderer::SetClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        Forge::Renderer::Clear();
    }

    void OnShutdown() override {
        FE_INFO("Sandbox: OnShutdown()");
    }
};

int main() {
    SandboxApp app;
    app.Run();
    return 0;
}
