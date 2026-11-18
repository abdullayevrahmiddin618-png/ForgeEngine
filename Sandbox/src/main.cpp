#include <Forge/Forge.h>
#include <memory>

class SandboxApp : public Forge::Application {
public:
    SandboxApp()
        : Forge::Application({ "ForgeEngine - Sandbox", 1280, 720 }) {}

protected:
    void OnInit() override {
        FE_INFO("Sandbox: OnInit()");

        const std::string vertexSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Color;
            out vec3 v_Color;
            void main() {
                v_Color = a_Color;
                gl_Position = vec4(a_Position, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_Color;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(v_Color, 1.0);
            }
        )";

        m_Shader = std::make_unique<Forge::Shader>(vertexSrc, fragmentSrc);

        m_Triangle = std::make_unique<Forge::VertexArray>(std::vector<Forge::Vertex>{
            //  x,     y,    z,     r,    g,    b
            { -0.5f, -0.5f, 0.0f,  1.0f, 0.2f, 0.2f }, // Qizil
            {  0.5f, -0.5f, 0.0f,  0.2f, 1.0f, 0.2f }, // Yashil
            {  0.0f,  0.5f, 0.0f,  0.2f, 0.4f, 1.0f }  // Ko'k
        });
    }

    void OnUpdate() override {
        Forge::Renderer::SetClearColor(0.1f, 0.12f, 0.15f, 1.0f);
        Forge::Renderer::Clear();
        Forge::Renderer::Draw(*m_Triangle, *m_Shader);
    }

    void OnShutdown() override {
        m_Triangle.reset();
        m_Shader.reset();
        FE_INFO("Sandbox: OnShutdown()");
    }

private:
    std::unique_ptr<Forge::Shader> m_Shader;
    std::unique_ptr<Forge::VertexArray> m_Triangle;
};

int main() {
    SandboxApp app;
    app.Run();
    return 0;
}
