#include <Forge/Forge.h>
#include <memory>
#include <string>

class SandboxApp : public Forge::Application {
public:
    SandboxApp()
        : Forge::Application({ "ForgeEngine - 3D Editor", 1280, 720 }),
          m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f) {}

protected:
    void OnInit() override {
        FE_INFO("Sandbox: Scene va 3D Editor yuklanmoqda...");
        m_Camera.SetPosition({ 0.0f, 1.5f, 5.0f });

        const std::string vertexSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Color;
            layout (location = 2) in vec3 a_Normal;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;

            out vec3 v_Normal;
            out vec3 v_FragPos;

            void main() {
                v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_Normal;
            in vec3 v_FragPos;

            uniform vec3 u_ObjectColor;
            uniform vec3 u_LightPos;
            uniform vec3 u_LightColor;
            uniform vec3 u_ViewPos;
            uniform float u_AmbientStrength;
            uniform float u_SpecularStrength;

            out vec4 FragColor;

            void main() {
                vec3 ambient = u_AmbientStrength * u_LightColor;

                vec3 norm = normalize(v_Normal);
                vec3 lightDir = normalize(u_LightPos - v_FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                vec3 diffuse = diff * u_LightColor;

                vec3 viewDir = normalize(u_ViewPos - v_FragPos);
                vec3 reflectDir = reflect(-lightDir, norm);
                float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
                vec3 specular = u_SpecularStrength * spec * u_LightColor;

                vec3 result = (ambient + diffuse + specular) * u_ObjectColor;
                FragColor = vec4(result, 1.0);
            }
        )";

        m_Shader = std::make_unique<Forge::Shader>(vertexSrc, fragmentSrc);

        m_CubeMesh = std::make_unique<Forge::VertexArray>(std::vector<Forge::Vertex>{
            // Old (0, 0, 1)
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f },
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f,  0.0f,  1.0f },

            // Orqa (0, 0, -1)
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f },
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f,  0.0f, -1.0f },

            // Chap (-1, 0, 0)
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f },
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f },
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,  -1.0f,  0.0f,  0.0f },

            // O'ng (1, 0, 0)
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   1.0f,  0.0f,  0.0f },

            // Tepa (0, 1, 0)
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f },
            {  0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f },
            {  0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f },
            { -0.5f,  0.5f,  0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f },
            { -0.5f,  0.5f, -0.5f,  1, 1, 1,   0.0f,  1.0f,  0.0f },

            // Past (0, -1, 0)
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f },
            {  0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f },
            {  0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f },
            { -0.5f, -0.5f,  0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f },
            { -0.5f, -0.5f, -0.5f,  1, 1, 1,   0.0f, -1.0f,  0.0f }
        });

        // Boshlang'ich sahna obyektlari
        auto& floor = m_Scene.CreateEntity("Floor");
        floor.TransformData.Position = { 0.0f, -0.75f, 0.0f };
        floor.TransformData.Scale    = { 6.0f, 0.2f, 6.0f };
        floor.Color                  = { 0.25f, 0.28f, 0.32f };

        auto& cube1 = m_Scene.CreateEntity("Main Cube");
        cube1.TransformData.Position = { -0.8f, 0.0f, 0.0f };
        cube1.Color                  = { 0.9f, 0.35f, 0.2f };

        auto& cube2 = m_Scene.CreateEntity("Side Cube");
        cube2.TransformData.Position = { 1.0f, 0.2f, -0.5f };
        cube2.TransformData.Rotation = { 0.0f, 35.0f, 0.0f };
        cube2.Color                  = { 0.2f, 0.6f, 0.95f };
    }

    void OnUpdate(float dt) override {
        m_LastDt = dt;

        if (Forge::Input::IsKeyPressed(Forge::Key::Escape)) {
            Close();
        }

        float speed = 3.0f * dt;
        if (Forge::Input::IsKeyPressed(Forge::Key::W)) m_Camera.Move({ 0.0f,  0.0f, -speed });
        if (Forge::Input::IsKeyPressed(Forge::Key::S)) m_Camera.Move({ 0.0f,  0.0f,  speed });
        if (Forge::Input::IsKeyPressed(Forge::Key::A)) m_Camera.Move({ -speed, 0.0f,  0.0f });
        if (Forge::Input::IsKeyPressed(Forge::Key::D)) m_Camera.Move({  speed, 0.0f,  0.0f });
        if (Forge::Input::IsKeyPressed(Forge::Key::E)) m_Camera.Move({ 0.0f,  speed,  0.0f });
        if (Forge::Input::IsKeyPressed(Forge::Key::Q)) m_Camera.Move({ 0.0f, -speed,  0.0f });

        // 1-kubni (Main Cube) sekin aylantirish
        auto& entities = m_Scene.GetEntities();
        if (m_AutoRotate && entities.size() > 1) {
            entities[1].TransformData.Rotation.y += 35.0f * dt;
        }

        Forge::Renderer::SetClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
        Forge::Renderer::Clear();

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        m_Shader->SetVec3("u_LightPos", m_LightPos);
        m_Shader->SetVec3("u_LightColor", m_LightColor);
        m_Shader->SetVec3("u_ViewPos", m_Camera.GetPosition());
        m_Shader->SetFloat("u_AmbientStrength", m_AmbientStrength);
        m_Shader->SetFloat("u_SpecularStrength", m_SpecularStrength);

        for (const auto& entity : entities) {
            if (!entity.Visible) continue;
            m_Shader->SetMat4("u_Model", entity.TransformData.GetModelMatrix());
            m_Shader->SetVec3("u_ObjectColor", entity.Color);
            Forge::Renderer::Draw(*m_CubeMesh, *m_Shader);
        }
    }

    void OnImGuiRender() override {
        // 1. Scene Hierarchy oynasi
        ImGui::Begin("Scene Hierarchy");

        if (ImGui::Button("+ Yangi Kub")) {
            auto& e = m_Scene.CreateEntity("Cube " + std::to_string(m_Scene.GetEntities().size()));
            e.TransformData.Position = { 0.0f, 1.0f, 0.0f };
            m_SelectedIndex = static_cast<int>(m_Scene.GetEntities().size()) - 1;
        }

        ImGui::Separator();
        auto& entities = m_Scene.GetEntities();
        for (int i = 0; i < static_cast<int>(entities.size()); ++i) {
            bool isSelected = (m_SelectedIndex == i);
            if (ImGui::Selectable(entities[i].Name.c_str(), isSelected)) {
                m_SelectedIndex = i;
            }
        }
        ImGui::End();

        // 2. Inspector oynasi
        ImGui::Begin("Inspector");
        if (m_SelectedIndex >= 0 && m_SelectedIndex < static_cast<int>(entities.size())) {
            auto& e = entities[m_SelectedIndex];
            ImGui::Text("Obyekt: %s", e.Name.c_str());
            ImGui::Checkbox("Ko'rinadigan", &e.Visible);
            ImGui::DragFloat3("Position", &e.TransformData.Position.x, 0.05f);
            ImGui::DragFloat3("Rotation", &e.TransformData.Rotation.x, 1.0f);
            ImGui::DragFloat3("Scale",    &e.TransformData.Scale.x,    0.05f);
            ImGui::ColorEdit3("Rang",     &e.Color.x);

            if (ImGui::Button("O'chirish")) {
                m_Scene.RemoveEntity(static_cast<size_t>(m_SelectedIndex));
                m_SelectedIndex = -1;
            }
        } else {
            ImGui::Text("Hierarchy dan obyekt tanlang.");
        }
        ImGui::End();

        // 3. Sahna & Yorug'lik sozlamalari
        ImGui::Begin("Engine Settings");
        float fps = (m_LastDt > 0.0f) ? (1.0f / m_LastDt) : 0.0f;
        ImGui::Text("FPS: %.1f (%.2f ms)", fps, m_LastDt * 1000.0f);
        ImGui::Checkbox("Auto-Rotate Main Cube", &m_AutoRotate);
        ImGui::ColorEdit3("Fon rangi", m_ClearColor);
        ImGui::Separator();
        ImGui::DragFloat3("Yorug'lik joyi", &m_LightPos.x, 0.1f);
        ImGui::ColorEdit3("Yorug'lik rangi", &m_LightColor.x);
        ImGui::SliderFloat("Ambient", &m_AmbientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular", &m_SpecularStrength, 0.0f, 2.0f);
        ImGui::End();
    }

    void OnShutdown() override {
        m_CubeMesh.reset();
        m_Shader.reset();
        FE_INFO("Sandbox: OnShutdown()");
    }

private:
    Forge::Camera m_Camera;
    Forge::Scene m_Scene;
    std::unique_ptr<Forge::Shader> m_Shader;
    std::unique_ptr<Forge::VertexArray> m_CubeMesh;

    int m_SelectedIndex = 1;
    bool m_AutoRotate = true;
    float m_LastDt = 0.0f;
    float m_ClearColor[3] = { 0.08f, 0.1f, 0.13f };

    glm::vec3 m_LightPos{ 2.5f, 3.0f, 2.5f };
    glm::vec3 m_LightColor{ 1.0f, 1.0f, 1.0f };
    float m_AmbientStrength = 0.2f;
    float m_SpecularStrength = 0.5f;
};

int main() {
    SandboxApp app;
    app.Run();
    return 0;
}
