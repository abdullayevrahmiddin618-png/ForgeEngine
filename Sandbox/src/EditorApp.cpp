#include "EditorApp.h"

EditorApp::EditorApp()
    : Forge::Application({ "ForgeEngine - 3D Editor", 1280, 720 }),
      m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f) {}

void EditorApp::OnInit() {
    FE_INFO("ForgeEngine Editor ishga tushmoqda...");
    ResetCamera();

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
            FragColor = vec4((ambient + diffuse + specular) * baseColor, 1.0);
        }
    )";

    m_Shader      = std::make_unique<Forge::Shader>(vertexSrc, fragmentSrc);
    m_Texture     = Forge::Texture::CreateCheckerboard(256, 32);
    m_CubeMesh    = Forge::Mesh::CreateCube();
    m_SphereMesh  = Forge::Mesh::CreateSphere(32, 20);
    m_PyramidMesh = Forge::Mesh::CreatePyramid();
    m_PlaneMesh   = Forge::Mesh::CreatePlane();
    m_Framebuffer = std::make_unique<Forge::Framebuffer>(800, 450);
    m_Grid        = std::make_unique<Forge::Grid>();

    SetupDefaultScene();
    EditorUI::PushLog(m_State, "[INIT] ForgeEngine tayyor. Fizika yoqilgan — Play bosib sinang!");
}

void EditorApp::OnUpdate(float dt) {
    m_State.LastDt = dt;

    if (Forge::Input::IsKeyPressed(Forge::Key::Escape)) {
        Close();
    }

    uint32_t vpW = static_cast<uint32_t>(m_State.ViewportSize.x);
    uint32_t vpH = static_cast<uint32_t>(m_State.ViewportSize.y);
    if (vpW > 0 && vpH > 0 && (vpW != m_Framebuffer->GetWidth() || vpH != m_Framebuffer->GetHeight())) {
        m_Framebuffer->Resize(vpW, vpH);
        m_Camera.SetAspectRatio(static_cast<float>(vpW) / static_cast<float>(vpH));
    }

    auto [mouseX, mouseY] = Forge::Input::GetMousePosition();
    if (Forge::Input::IsMouseButtonPressed(Forge::Mouse::Right) && (m_State.ViewportHovered || m_IsLooking)) {
        if (!m_IsLooking) {
            m_LastMouseX = mouseX;
            m_LastMouseY = mouseY;
            m_IsLooking  = true;
        }
        float dx = mouseX - m_LastMouseX;
        float dy = m_LastMouseY - mouseY;
        m_LastMouseX = mouseX;
        m_LastMouseY = mouseY;
        m_Camera.Rotate(dx * m_State.MouseSensitivity, dy * m_State.MouseSensitivity);
    } else {
        m_IsLooking = false;
    }

    if (!ImGui::GetIO().WantTextInput && (m_State.ViewportHovered || m_IsLooking)) {
        float speed = m_State.CameraSpeed * dt;
        if (Forge::Input::IsKeyPressed(Forge::Key::LeftShift)) speed *= 2.0f;
        if (Forge::Input::IsKeyPressed(Forge::Key::W)) m_Camera.Move( m_Camera.GetFront() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::S)) m_Camera.Move(-m_Camera.GetFront() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::D)) m_Camera.Move( m_Camera.GetRight() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::A)) m_Camera.Move(-m_Camera.GetRight() * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::E)) m_Camera.Move( m_Camera.GetUp()    * speed);
        if (Forge::Input::IsKeyPressed(Forge::Key::Q)) m_Camera.Move(-m_Camera.GetUp()    * speed);
    }

    auto& entities = m_Scene.GetEntities();
    if (m_State.IsPlaying && !m_State.IsPaused) {
        Forge::PhysicsWorld::Step(m_Scene, dt);
    }

    m_Framebuffer->Bind();
    const auto& w = m_State.World;
    Forge::Renderer::SetClearColor(w.ClearColor.x, w.ClearColor.y, w.ClearColor.z, 1.0f);
    Forge::Renderer::Clear();

    m_Shader->Bind();
    m_Shader->SetMat4("u_ViewProjection", m_Camera.GetViewProjectionMatrix());
    m_Shader->SetVec3("u_LightPos", w.LightPos);
    m_Shader->SetVec3("u_LightColor", w.LightColor);
    m_Shader->SetVec3("u_ViewPos", m_Camera.GetPosition());
    m_Shader->SetFloat("u_AmbientStrength", w.AmbientStrength);
    m_Shader->SetFloat("u_SpecularStrength", w.SpecularStrength);
    m_Shader->SetInt("u_Texture", 0);

    for (const auto& entity : entities) {
        if (!entity.Visible) continue;

        const Forge::Texture* customTex = GetEntityTexture(entity);
        if (customTex) {
            customTex->Bind(0);
            m_Shader->SetInt("u_UseTexture", 1);
        } else {
            m_Texture->Bind(0);
            m_Shader->SetInt("u_UseTexture", w.UseTexture ? 1 : 0);
        }

        m_Shader->SetMat4("u_Model", entity.TransformData.GetModelMatrix());
        m_Shader->SetVec3("u_ObjectColor", entity.Color);
        Forge::Renderer::Draw(GetEntityMesh(entity), *m_Shader);
    }

    if (m_State.ShowGrid) {
        m_Grid->Draw(m_Camera);
    }

    m_Framebuffer->Unbind();
    m_State.ViewportTextureID = m_Framebuffer->GetColorAttachmentID();

    Forge::Renderer::SetClearColor(0.05f, 0.05f, 0.06f, 1.0f);
    Forge::Renderer::Clear();
}

void EditorApp::OnImGuiRender() {
    EditorCallbacks cb;
    cb.OnNewScene = [this]() {
        while (!m_Scene.GetEntities().empty()) m_Scene.RemoveEntity(0);
        SetupDefaultScene();
        EditorUI::PushLog(m_State, "[SCENE] Yangi sahna yaratildi.");
    };
    cb.OnSaveScene = [this]() {
        if (SceneSerializer::Save("scene.forge", m_Scene, m_State.World))
            EditorUI::PushLog(m_State, "[SAVE] Sahna 'scene.forge' fayliga saqlandi.");
        else
            EditorUI::PushLog(m_State, "[ERROR] Saqlashda xato!");
    };
    cb.OnLoadScene = [this]() {
        if (SceneSerializer::Load("scene.forge", m_Scene, m_State.World)) {
            m_State.SelectedIndex = m_Scene.GetEntities().empty() ? -1 : 0;
            EditorUI::PushLog(m_State, "[LOAD] 'scene.forge' muvaffaqiyatli yuklandi.");
        } else {
            EditorUI::PushLog(m_State, "[ERROR] 'scene.forge' topilmadi. Avval Save bosing.");
        }
    };
    cb.OnPlay = [this]() {
        if (!m_State.IsPlaying) {
            SceneSerializer::Save(".play_backup.forge", m_Scene, m_State.World);
            m_State.IsPlaying = true;
            m_State.IsPaused  = false;
            EditorUI::PushLog(m_State, "[PLAY] Fizika simulyatsiyasi boshlandi.");
        } else {
            m_State.IsPaused = false;
        }
    };
    cb.OnPause = [this]() {
        if (m_State.IsPlaying) {
            m_State.IsPaused = !m_State.IsPaused;
            EditorUI::PushLog(m_State, m_State.IsPaused ? "[PAUSE] To'xtatildi." : "[PLAY] Davom etdi.");
        }
    };
    cb.OnStop = [this]() {
        if (m_State.IsPlaying) {
            m_State.IsPlaying = false;
            m_State.IsPaused  = false;
            SceneSerializer::Load(".play_backup.forge", m_Scene, m_State.World);
            EditorUI::PushLog(m_State, "[STOP] Sahna o'yindan oldingi holatiga qaytarildi.");
        }
    };
    cb.OnResetCamera = [this]() {
        ResetCamera();
        EditorUI::PushLog(m_State, "[CAMERA] Kamera qaytarildi.");
    };
    cb.OnExit = [this]() { Close(); };
    cb.OnAddEntity = [this](const std::string& name, Forge::MeshType type) {
        AddEntity(name, type);
    };

    m_UI.Render(m_Scene, m_Camera, m_State, cb);
}

void EditorApp::OnShutdown() {
    m_CustomTextures.clear();
    m_CustomMeshes.clear();
    m_Grid.reset();
    m_Framebuffer.reset();
    m_PlaneMesh.reset();
    m_PyramidMesh.reset();
    m_SphereMesh.reset();
    m_CubeMesh.reset();
    m_Texture.reset();
    m_Shader.reset();
}

void EditorApp::SetupDefaultScene() {
    auto& floor = m_Scene.CreateEntity("Floor");
    floor.Shape                  = Forge::MeshType::Plane;
    floor.TransformData.Position = { 0.0f, -0.5f, 0.0f };
    floor.TransformData.Scale    = { 10.0f, 1.0f, 10.0f };
    floor.Color                  = { 0.45f, 0.50f, 0.55f };
    floor.PhysicsEnabled         = true;
    floor.IsStatic               = true;

    auto& cube = m_Scene.CreateEntity("MainCube");
    cube.Shape                  = Forge::MeshType::Cube;
    cube.TransformData.Position = { -1.5f, 2.2f, 0.0f };
    cube.Color                  = { 0.95f, 0.45f, 0.30f };
    cube.PhysicsEnabled         = true;
    cube.Bounciness             = 0.45f;

    auto& sphere = m_Scene.CreateEntity("Sphere");
    sphere.Shape                  = Forge::MeshType::Sphere;
    sphere.TransformData.Position = { 0.0f, 3.2f, 0.0f };
    sphere.Color                  = { 0.30f, 0.70f, 1.00f };
    sphere.PhysicsEnabled         = true;
    sphere.Bounciness             = 0.70f;

    auto& pyramid = m_Scene.CreateEntity("Pyramid");
    pyramid.Shape                  = Forge::MeshType::Pyramid;
    pyramid.TransformData.Position = { 1.5f, 1.8f, 0.0f };
    pyramid.Color                  = { 0.95f, 0.80f, 0.25f };
    pyramid.PhysicsEnabled         = true;
    pyramid.Bounciness             = 0.25f;

    m_State.SelectedIndex = 1;
}

void EditorApp::ResetCamera() {
    m_Camera.SetPosition({ 0.0f, 2.2f, 6.5f });
    m_Camera.SetRotation(-90.0f, -16.0f);
}

void EditorApp::AddEntity(const std::string& baseName, Forge::MeshType type) {
    std::string name = baseName + "_" + std::to_string(m_Scene.GetEntities().size());
    auto& e = m_Scene.CreateEntity(name);
    e.Shape = type;
    e.TransformData.Position = { 0.0f, 2.0f, 0.0f };
    e.PhysicsEnabled = true;
    m_State.SelectedIndex = static_cast<int>(m_Scene.GetEntities().size()) - 1;
    EditorUI::PushLog(m_State, "[ENTITY] Qo'shildi: " + name);
}

const Forge::VertexArray& EditorApp::GetEntityMesh(const Forge::Entity& entity) {
    if (entity.Shape == Forge::MeshType::Custom && !entity.MeshPath.empty()) {
        auto it = m_CustomMeshes.find(entity.MeshPath);
        if (it == m_CustomMeshes.end()) {
            auto loaded = Forge::Mesh::LoadOBJ(entity.MeshPath);
            it = m_CustomMeshes.emplace(entity.MeshPath, std::move(loaded)).first;
        }
        if (it->second) {
            return *it->second;
        }
    }

    switch (entity.Shape) {
        case Forge::MeshType::Sphere:  return *m_SphereMesh;
        case Forge::MeshType::Pyramid: return *m_PyramidMesh;
        case Forge::MeshType::Plane:   return *m_PlaneMesh;
        case Forge::MeshType::Cube:
        default:                       return *m_CubeMesh;
    }
}

const Forge::Texture* EditorApp::GetEntityTexture(const Forge::Entity& entity) {
    if (entity.TexturePath.empty()) return nullptr;

    auto it = m_CustomTextures.find(entity.TexturePath);
    if (it == m_CustomTextures.end()) {
        auto tex = std::make_unique<Forge::Texture>(entity.TexturePath);
        if (tex->GetWidth() == 0) {
            tex.reset();
        }
        it = m_CustomTextures.emplace(entity.TexturePath, std::move(tex)).first;
    }
    return it->second.get();
}
