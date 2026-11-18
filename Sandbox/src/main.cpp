#include <Forge/Forge.h>

class SandboxApp : public Forge::Application {
protected:
    void OnInit() override {
        FE_INFO("Sandbox: OnInit()");
    }

    void OnUpdate() override {
        FE_INFO("Sandbox: OnUpdate()");
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
