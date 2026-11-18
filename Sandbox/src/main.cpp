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
        // Har kadrda ishlaydi
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
