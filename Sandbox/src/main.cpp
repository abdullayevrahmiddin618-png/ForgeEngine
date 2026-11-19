#include <Forge/Forge.h>
#include <memory>

class SandboxApp : public Forge::Application {
public:
    SandboxApp()
        : Forge::Application({ "ForgeEngine - 3D Sandbox", 1280, 720 }),
          m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f) {}

protected:
    void OnInit() override {
        FE_INFO("Sandbox: 3D Kub + Phong Lighting yuklanmoqda...");

        const std::string vertexSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Color;
            layout (location = 2) in vec3 a_Normal;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;

            out vec3 v_Color;
            out vec3 v_Normal;
            out vec3 v_FragPos;

            void main() {
                v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                v_Color = a_Color;
                gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_Color;
            in vec3 v_Normal;
            in vec3 v_FragPos;

            uniform vec3 u_LightPos;
            uniform vec3 u_LightColor;
            uniform vec3 u_ViewPos;
            uniform float u_AmbientStrength;
            uniform float u_SpecularStrength;

            out vec4 FragColor;

            void main() {
                // 1. Ambient (umumiy fon yorug'ligi)
                vec3 ambient = u_AmbientStrength * u_LightColor;

                // 2. Diffuse (to'g'ridan-to'g'ri yorug'lik)
                vec3 norm = normalize(v_Normal);
                vec3 lightDir = normalize(u_LightPos - v_FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * u_LightColor;

                // 3. Specular (yaltirash)
                vec3 viewDir = normalize(u_ViewPos - v_FragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                vec3 specular = u_SpecularStrength * spec * u_LightColor;

                vec3 result = (ambient + diffuse + specular) * v_Color;
                FragColor = vec4(result, 1.0);
            }
        )";

        m_Shader = std::make_unique<Forge::Shader>(vertexSrc, fragmentSrc);

        m_Cube = std::make_unique<Forge::VertexArray>(std::vector<Forge::Vertex>{
            // Old tomon (Normal: 0, 0, 1)
            { -0.5f, -0.5f,  0.5f,  0.9f, 0.3f, 0.2f,   0.0f,  0.0f,  1.0f },
            {  0.5f, -0.5f,  0.5f,  0.9f, 0.3f, 0.2f,   0.0f,  0.0f,  1.0f },
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.2f,   0.0f,  0.0f,  1.0f },
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.2f,   0.0f,  0.0f,  1.0f },
            { -0.5f,  0.5f,  0.5f,  0.9f, 0.3f, 0.2f,   0.0f,  0.0f,  1.0f },
            { -0.5f, -0.5f,  0.5f,  0.9f, 0.3f, 0.2f,   0.0f,  0.0f,  1.0f },

            // Orqa tomon (Normal: 0, 0, -1)
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 0.3f,   0.0f,  0.0f, -1.0f },
            {  0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 0.3f,   0.0f,  0.0f, -1.0f },
            {  0.5f,  0.5f, -0.5f,  0.2f, 0.8f, 0.3f,   0.0f,  0.0f, -1.0f },
            {  0.5f,  0.5f, -0.5f,  0.2f, 0.8f, 0.3f,   0.0f,  0.0f, -1.0f },
            { -0.5f,  0.5f, -0.5f,  0.2f, 0.8f, 0.3f,   0.0f,  0.0f, -1.0f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 0.3f,   0.0f,  0.0f, -1.0f },

            // Chap tomon (Normal: -1, 0, 0)
            { -0.5f,  0.5f,  0.5f,  0.2f, 0.4f, 0.9f,  -1.0f,  0.0f,  0.0f },
            { -0.5f,  0.5f, -0.5f,  0.2f, 0.4f, 0.9f,  -1.0f,  0.0f,  0.0f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.9f,  -1.0f,  0.0f,  0.0f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.4f, 0.9f,  -1.0f,  0.0f,  0.0f },
            { -0.5f, -0.5f,  0.5f,  0.2f, 0.4f, 0.9f,  -1.0f,  0.0f,  0.0f },
            { -0.5f,  0.5f,  0.5f,  0.2f, 0.4f, 0.9f,  -1.0f,  0.0f,  0.0f },

            // O'ng tomon (Normal: 1, 0, 0)
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.8f, 0.2f,   1.0f,  0.0f,  0.0f },
            {  0.5f,  0.5f, -0.5f,  0.9f, 0.8f, 0.2f,   1.0f,  0.0f,  0.0f },
            {  0.5f, -0.5f, -0.5f,  0.9f, 0.8f, 0.2f,   1.0f,  0.0f,  0.0f },
            {  0.5f, -0.5f, -0.5f,  0.9f, 0.8f, 0.2f,   1.0f,  0.0f,  0.0f },
            {  0.5f, -0.5f,  0.5f,  0.9f, 0.8f, 0.2f,   1.0f,  0.0f,  0.0f },
            {  0.5f,  0.5f,  0.5f,  0.9f, 0.8f, 0.2f,   1.0f,  0.0f,  0.0f },

            // Tepa tomon (Normal: 0, 1, 0)
            { -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.9f,   0.0f,  1.0f,  0.0f },
            {  0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.9f,   0.0f,  1.0f,  0.0f },
            {  0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.9f,   0.0f,  1.0f,  0.0f },
            {  0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.9f,   0.0f,  1.0f,  0.0f },
            { -0.5f,  0.5f,  0.5f,  0.7f, 0.2f, 0.9f,   0.0f,  1.0f,  0.0f },
            { -0.5f,  0.5f, -0.5f,  0.7f, 0.2f, 0.9f,   0.0f,  1.0f,  0.0f },

            // Past tomon (Normal: 0, -1, 0)
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 0.9f,   0.0f, -1.0f,  0.0f },
            {  0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 0.9f,   0.0f, -1.0f,  0.0f },
            {  0.5f, -0.5f,  0.5f,  0.2f, 0.8f, 0.9f,   0.0f, -1.0f,  0.0f },
            {  0.5f, -0.5f,  0.5f,  0.2f, 0.8f, 0.9f,   0.0f, -1.0f,  0.0f },
            { -0.5f, -0.5f,  0.5f,  0.2f, 0.8f, 0.9f,   0.0f, -1.0f,  0.0f },
            { -0.5f, -0.5f, -0.5f,  0.2f, 0.8f, 0.9f,   0.0f, -1.0f,  0.0f }
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
        m_Shader->SetVec3("u_LightPos", m_LightPos);
        m_Shader->SetVec3("u_LightColor", m_LightColor);
        m_Shader->SetVec3("u_ViewPos", m_Camera.GetPosition());
        m_Shader->SetFloat("u_AmbientStrength", m_AmbientStrength);
        m_Shader->SetFloat("u_SpecularStrength", m_SpecularStrength);

        Forge::Renderer::Draw(*m_Cube, *m_Shader);
    }

    void OnImGuiRender() override {
        ImGui::Begin("ForgeEngine Inspector");

        float fps = (m_LastDt > 0.0f) ? (1.0f / m_LastDt) : 0.0f;
        ImGui::Text("FPS: %.1f (%.2f ms)", fps, m_LastDt * 1000.0f);
        ImGui::Separator();

        ImGui::ColorEdit3("Fon rangi", m_ClearColor);
        ImGui::SliderFloat("Aylanish tezligi", &m_RotationSpeed, 0.0f, 200.0f);

        ImGui::Separator();
        ImGui::Text("Yorug'lik (Phong Lighting)");
        ImGui::DragFloat3("Yorug'lik joyi", &m_LightPos.x, 0.1f);
        ImGui::ColorEdit3("Yorug'lik rangi", &m_LightColor.x);
        ImGui::SliderFloat("Ambient", &m_AmbientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular", &m_SpecularStrength, 0.0f, 2.0f);

        ImGui::Separator();
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
    float m_RotationSpeed = 40.0f;
    float m_LastDt = 0.0f;
    float m_ClearColor[3] = { 0.08f, 0.1f, 0.13f };

    glm::vec3 m_LightPos{ 2.0f, 2.0f, 2.0f };
    glm::vec3 m_LightColor{ 1.0f, 1.0f, 1.0f };
    float m_AmbientStrength = 0.2f;
    float m_SpecularStrength = 0.6f;
};

int main() {
    SandboxApp app;
    app.Run();
    return 0;
}
