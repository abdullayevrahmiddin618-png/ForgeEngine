#include <Forge/Forge.h>
#include <memory>

class SandboxApp : public Forge::Application {
public:
    SandboxApp()
        : Forge::Application({ "ForgeEngine - 3D Sandbox", 1280, 720 }),
          m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f) {}

protected:
    void OnInit() override {
        FE_INFO("Sandbox: 3D Kub yuklanmoqda...");

        const std::string vertexSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Color;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;

            out vec3 v_Color;

            void main() {
                v_Color = a_Color;
                gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
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

        m_Cube = std::make_unique<Forge::VertexArray>(std::vector<Forge::Vertex>{
            // Old tomon (Qizil)
            { -0.5f, -0.5f,  0.5f,  0.9f, 0.2f, 0.2f },
            {  0.5f, -0.5f,  0.5f,  0.9f, 0.2f, 0.2f },
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.2f, 0.2f },
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.2f, 0.2f },
            { -0.5f,  0.5f,  0.5f,  0.9f, 0.2f, 0.2f },
            { -0.5f, -0.5f,  0.5f,  0.9f, 0.2f, 0.2f },

            // Orqa tomon (Yashil)
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.9f, 0.2f },
            {  0.5f, -0.5f, -0.5f,  0.2f, 0.9f, 0.2f },
            {  0.5f,  0.5f, -0.5f,  0.2f, 0.9f, 0.2f },
            {  0.5f,  0.5f, -0.5f,  0.2f, 0.9f, 0.2f },
            { -0.5f,  0.5f, -0.5f,  0.2f, 0.9f, 0.2f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.9f, 0.2f },

            // Chap tomon (Ko'k)
            { -0.5f,  0.5f,  0.5f,  0.2f, 0.4f, 0.9f },
            { -0.5f,  0.5f, -0.5f,  0.2f, 0.4f, 0.9f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.9f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.9f },
            { -0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.9f },
            { -0.5f,  0.5f,  0.5f,  0.2f, 0.4f, 0.9f },

            // O'ng tomon (Sariq)
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.9f, 0.2f },
            {  0.5f,  0.5f, -0.5f,  0.9f, 0.9f, 0.2f },
            {  0.5f, -0.5f, -0.5f,  0.9f, 0.9f, 0.2f },
            {  0.5f, -0.5f, -0.5f,  0.9f, 0.9f, 0.2f },
            {  0.5f, -0.5f,  0.5f,  0.9f, 0.9f, 0.2f },
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.9f, 0.2f },

            // Tepa tomon (Binafsha)
            { -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.9f },
            {  0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.9f },
            {  0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.9f },
            {  0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.9f },
            { -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.9f },
            { -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.9f },

            // Past tomon (Moviy)
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.9f, 0.9f },
            {  0.5f, -0.5f, -0.5f,  0.2f, 0.9f, 0.9f },
            {  0.5f, -0.5f,  0.5f,  0.2f, 0.9f, 0.9f },
            {  0.5f, -0.5f,  0.5f,  0.2f, 0.9f, 0.9f },
            { -0.5f, -0.5f,  0.5f,  0.2f, 0.9f, 0.9f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.9f, 0.9f }
        });
    }

    void OnUpdate(float dt) override {
        m_LastDt = dt;

        if (Forge::Input::IsKeyPressed(Forge::Key::Escape)) {
            Close();
        }

        float speed = 2.5f * dt;
        if (Forge::Input::IsKeyPressed(Forge::Key::W)) m_Camera.Move({ 0.0f,  0.0f, -speed });
        if (Forge::Input::IsKeyPressed(Forge::Key::S)) m_Camera.Move({ 0.0f,  0.0f,  speed });
        if (Forge::Input::IsKeyPressed(Forge::Key::A)) m_Camera.Move({ -speed, 0.0f,  0.0f });
        if (Forge::Input::IsKeyPressed(Forge::Key::D)) m_Camera.Move({  speed, 0.0f,  0.0f });
        if (Forge::Input::IsKeyPressed(Forge::Key::E)) m_Camera.Move({ 0.0f,  speed,  0.0f });
        if (Forge::Input::IsKeyPressed(Forge::Key::Q)) m_Camera.Move({ 0.0f, -speed,  0.0f });

        m_Rotation += m_RotationSpeed * dt;
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.5f, 1.0f, 0.0f));

        Forge::Renderer::SetClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
        Forge::Renderer::Clear();

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        m_Shader->SetMat4("u_Model", model);

        Forge::Renderer::Draw(*m_Cube, *m_Shader);
    }

    void OnImGuiRender() override {
        ImGui::Begin("ForgeEngine Inspector");

        float fps = (m_LastDt > 0.0f) ? (1.0f / m_LastDt) : 0.0f;
        ImGui::Text("FPS: %.1f (%.2f ms)", fps, m_LastDt * 1000.0f);
        ImGui::Separator();

        ImGui::ColorEdit3("Fon rangi", m_ClearColor);
        ImGui::SliderFloat("Aylanish tezligi", &m_RotationSpeed, 0.0f, 200.0f);

        auto pos = m_Camera.GetPosition();
        ImGui::Text("Kamera: X=%.2f  Y=%.2f  Z=%.2f", pos.x, pos.y, pos.z);
        if (ImGui::Button("Kamerani qaytarish")) {
            m_Camera.SetPosition({ 0.0f, 0.0f, 3.0f });
        }

        ImGui::End();
    }

    void OnShutdown() override {
        m_Cube.reset();
        m_Shader.reset();
        FE_INFO("Sandbox: OnShutdown()");
    }

private:
    Forge::Camera m_Camera;
    std::unique_ptr<Forge::Shader> m_Shader;
    std::unique_ptr<Forge::VertexArray> m_Cube;

    float m_Rotation = 0.0f;
    float m_RotationSpeed = 50.0f;
    float m_LastDt = 0.0f;
    float m_ClearColor[3] = { 0.08f, 0.1f, 0.13f };
};

int main() {
    SandboxApp app;
    app.Run();
    return 0;
}
