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
        FE_INFO("Sandbox: Scene, Mesh Generator va 3D Editor yuklanmoqda...");
        m_Camera.SetPosition({ 0.0f, 1.8f, 5.5f });
        m_Camera.SetRotation(-90.0f, -15.0f);

        const std::string vertexSrc = R"(
            #version 330 core
            layout (location = 0) in vec3 a_Position;
            layout (location = 1) in vec3 a_Color;
            layout (location = 2) in vec3 a_Normal;
            layout (location = 3) in vec2 a_TexCoord;

            uniform mat4 u_ViewProjection;
            uniform mat4 u_Model;

            out vec3 v_Normal;
            out vec3 v_FragPos;
            out vec2 v_TexCoord;

            void main() {
                v_FragPos = vec3(u_Model * vec4(a_Position, 1.0));
                v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
                v_TexCoord = a_TexCoord;
                gl_Position = u_ViewProjection * vec4(v_FragPos, 1.0);
            }
        )";

        const std::string fragmentSrc = R"(
            #version 330 core
            in vec3 v_Normal;
            in vec3 v_FragPos;
            in vec2 v_TexCoord;

            uniform vec3 u_ObjectColor;
            uniform vec3 u_LightPos;
            uniform vec3 u_LightColor;
            uniform vec3 u_ViewPos;
            uniform float u_AmbientStrength;
            uniform float u_SpecularStrength;
            uniform sampler2D u_Texture;
            uniform int u_UseTexture;

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

                vec3 baseColor = u_ObjectColor;
                if (u_UseTexture == 1) {
                    baseColor *= texture(u_Texture, v_TexCoord).rgb;
                }

                vec3 result = (ambient + diffuse + specular) * baseColor;
                FragColor = vec4(result, 1.0);
            }
        )";

        m_Shader = std::make_unique<Forge::Shader>(vertexSrc, fragmentSrc);
        m_Texture = Forge::Texture::CreateCheckerboard(256, 32);

        m_CubeMesh    = Forge::Mesh::CreateCube();
        m_SphereMesh  = Forge::Mesh::CreateSphere(32, 20);
        m_PyramidMesh = Forge::Mesh::CreatePyramid();
        m_PlaneMesh   = Forge::Mesh::CreatePlane();

        auto& floor = m_Scene.CreateEntity("Floor");
        floor.Shape                  = Forge::MeshType::Plane;
        floor.TransformData.Position = { 0.0f, -0.5f, 0.0f };
        floor.TransformData.Scale    = { 8.0f, 1.0f, 8.0f };
        floor.Color                  = { 0.45f, 0.5f, 0.55f };

        auto& cube = m_Scene.CreateEntity("Main Cube");
        cube.Shape                  = Forge::MeshType::Cube;
        cube.TransformData.Position = { -1.5f, 0.0f, 0.0f };
        cube.Color                  = { 0.95f, 0.45f, 0.3f };

        auto& sphere = m_Scene.CreateEntity("Sphere");
        sphere.Shape                  = Forge::MeshType::Sphere;
        sphere.TransformData.Position = { 0.0f, 0.0f, 0.0f };
        sphere.Color                  = { 0.3f, 0.7f, 1.0f };

        auto& pyramid = m_Scene.CreateEntity("Pyramid");
        pyramid.Shape                  = Forge::MeshType::Pyramid;
        pyramid.TransformData.Position = { 1.5f, 0.0f, 0.0f };
        pyramid.Color                  = { 0.95f, 0.8f, 0.25f };
    }

    void OnUpdate(float dt) override {
        m_LastDt = dt;

        if (Forge::Input::IsKeyPressed(Forge::Key::Escape)) {
            Close();
        }

        auto [mouseX, mouseY] = Forge::Input::GetMousePosition();
        if (Forge::Input::IsMouseButtonPressed(Forge::Mouse::Right) && !ImGui::GetIO().WantCaptureMouse) {
            if (!m_IsLooking) {
                m_LastMouseX = mouseX;
                m_LastMouseY = mouseY;
                m_IsLooking = true;
            }
            float dx = mouseX - m_LastMouseX;
            float dy = m_LastMouseY - mouseY;
            m_LastMouseX = mouseX;
            m_LastMouseY = mouseY;

            m_Camera.Rotate(dx * m_MouseSensitivity, dy * m_MouseSensitivity);
        } else {
            m_IsLooking = false;
        }

        float speed = m_CameraSpeed * dt;
        if (Forge::Input::IsKeyPressed(Forge::Key::LeftShift)) {
            speed *= 2.0f;
        }
        if (Forge::Input::IsKeyPressed(Forge::Key::W)) m_Camera.Move( m_Camera.GetFront() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::S)) m_Camera.Move(-m_Camera.GetFront() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::D)) m_Camera.Move( m_Camera.GetRight() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::A)) m_Camera.Move(-m_Camera.GetRight() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::E)) m_Camera.Move( m_Camera.GetUp()    * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::Q)) m_Camera.Move(-m_Camera.GetUp()    * speed);

        auto& entities = m_Scene.GetEntities();
        if (m_AutoRotate && entities.size() > 1) {
            entities[1].TransformData.Rotation.y += 35.0f * dt;
        }

        Forge::Renderer::SetClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
        Forge::Renderer::Clear();

        m_Texture->Bind(0);

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
        m_Shader->SetVec3("u_LightPos", m_LightPos);
        m_Shader->SetVec3("u_LightColor", m_LightColor);
        m_Shader->SetVec3("u_ViewPos", m_Camera.GetPosition());
        m_Shader->SetFloat("u_AmbientStrength", m_AmbientStrength);
        m_Shader->SetFloat("u_SpecularStrength", m_SpecularStrength);
        m_Shader->SetInt("u_Texture", 0);
        m_Shader->SetInt("u_UseTexture", m_UseTexture ? 1 : 0);

        for (const auto& entity : entities) {
            if (!entity.Visible) continue;
            m_Shader->SetMat4("u_Model", entity.TransformData.GetModelMatrix());
            m_Shader->SetVec3("u_ObjectColor", entity.Color);
            Forge::Renderer::Draw(GetMesh(entity.Shape), *m_Shader);
        }
    }

    void OnImGuiRender() override {
        // 1. Scene Hierarchy
        ImGui::Begin("Scene Hierarchy");
        if (ImGui::Button("+ Kub")) {
            AddEntity("Cube", Forge::MeshType::Cube);
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Sfera")) {
            AddEntity("Sphere", Forge::MeshType::Sphere);
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Piramida")) {
            AddEntity("Pyramid", Forge::MeshType::Pyramid);
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

        // 2. Inspector
        ImGui::Begin("Inspector");
        if (m_SelectedIndex >= 0 && m_SelectedIndex < static_cast<int>(entities.size())) {
            auto& e = entities[m_SelectedIndex];
            ImGui::Text("Obyekt: %s", e.Name.c_str());
            ImGui::Checkbox("Ko'rinadigan", &e.Visible);

            const char* shapes[] = { "Cube", "Sphere", "Pyramid", "Plane" };
            int currentShape = static_cast<int>(e.Shape);
            if (ImGui::Combo("Shakl", &currentShape, shapes, 4)) {
                e.Shape = static_cast<Forge::MeshType>(currentShape);
            }

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

        // 3. Engine & Camera Settings
        ImGui::Begin("Engine Settings");
        float fps = (m_LastDt > 0.0f) ? (1.0f / m_LastDt) : 0.0f;
        ImGui::Text("FPS: %.1f (%.2f ms)", fps, m_LastDt * 1000.0f);
        ImGui::Checkbox("Auto-Rotate Main Cube", &m_AutoRotate);
        ImGui::Checkbox("Tekstura yoqish", &m_UseTexture);
        ImGui::ColorEdit3("Fon rangi", m_ClearColor);

        ImGui::Separator();
        ImGui::Text("Kamera (O'ng tugma + WASD/QE, Shift=2x)");
        ImGui::SliderFloat("Tezlik", &m_CameraSpeed, 1.0f, 15.0f);
        ImGui::SliderFloat("Sezgirlik", &m_MouseSensitivity, 0.05f, 0.5f);
        if (ImGui::Button("Kamerani qaytarish")) {
            m_Camera.SetPosition({ 0.0f, 1.8f, 5.5f });
            m_Camera.SetRotation(-90.0f, -15.0f);
        }

        ImGui::Separator();
        ImGui::DragFloat3("Yorug'lik joyi", &m_LightPos.x, 0.1f);
        ImGui::ColorEdit3("Yorug'lik rangi", &m_LightColor.x);
        ImGui::SliderFloat("Ambient", &m_AmbientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular", &m_SpecularStrength, 0.0f, 2.0f);
        ImGui::End();
    }

    void OnShutdown() override {
        m_PlaneMesh.reset();
        m_PyramidMesh.reset();
        m_SphereMesh.reset();
        m_CubeMesh.reset();
        m_Texture.reset();
        m_Shader.reset();
        FE_INFO("Sandbox: OnShutdown()");
    }

private:
    void AddEntity(const std::string& baseName, Forge::MeshType type) {
        auto& e = m_Scene.CreateEntity(baseName + " " + std::to_string(m_Scene.GetEntities().size()));
        e.Shape = type;
        e.TransformData.Position = { 0.0f, 1.0f, 0.0f };
        m_SelectedIndex = static_cast<int>(m_Scene.GetEntities().size()) - 1;
    }

    const Forge::VertexArray& GetMesh(Forge::MeshType type) const {
        switch (type) {
            case Forge::MeshType::Sphere:  return *m_SphereMesh;
            case Forge::MeshType::Pyramid: return *m_PyramidMesh;
            case Forge::MeshType::Plane:   return *m_PlaneMesh;
            case Forge::MeshType::Cube:
            default:                       return *m_CubeMesh;
        }
    }

    Forge::Camera m_Camera;
    Forge::Scene m_Scene;
    std::unique_ptr<Forge::Shader> m_Shader;
    std::unique_ptr<Forge::Texture> m_Texture;

    std::unique_ptr<Forge::VertexArray> m_CubeMesh;
    std::unique_ptr<Forge::VertexArray> m_SphereMesh;
    std::unique_ptr<Forge::VertexArray> m_PyramidMesh;
    std::unique_ptr<Forge::VertexArray> m_PlaneMesh;

    int m_SelectedIndex = 1;
    bool m_AutoRotate = true;
    bool m_UseTexture = true;
    float m_LastDt = 0.0f;
    float m_ClearColor[3] = { 0.08f, 0.1f, 0.13f };

    bool m_IsLooking = false;
    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;
    float m_CameraSpeed = 3.5f;
    float m_MouseSensitivity = 0.15f;

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
