#include "EditorApp.h"
#include <filesystem>

EditorApp::EditorApp()
    : Forge::Application({ "ForgeEngine - 3D Editor", 1280, 720 }),
      m_Camera(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f) {}

void EditorApp::OnInit() {
    FE_INFO("Starting ForgeEngine Editor...");

    // Load clean Windows system font (Segoe UI)
    ImGuiIO& io = ImGui::GetIO();
    const char* fontPath = "C:/Windows/Fonts/segoeui.ttf";
    if (std::filesystem::exists(fontPath)) {
        io.Fonts->AddFontFromFileTTF(fontPath, 17.0f);
    }

    ResetCamera();

    m_SceneRenderer = std::make_unique<Forge::SceneRenderer>();
    m_Framebuffer   = std::make_unique<Forge::Framebuffer>(800, 450);

    SetupDefaultScene();
    EditorUI::PushLog(m_State, "[INIT] ForgeEngine ready. Controls: RMB + WASD/QE in Viewport.");
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

    if (m_State.IsPlaying && !m_State.IsPaused) {
        Forge::PhysicsWorld::Step(m_Scene, dt);
    }

    // Render 3D scene into Viewport Framebuffer
    m_Framebuffer->Bind();
    m_SceneRenderer->Render(m_Scene, m_Camera, m_State.World, m_State.ShowGrid);
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
        EditorUI::PushLog(m_State, "[SCENE] Created new scene.");
    };
    cb.OnSaveScene = [this]() {
        if (SceneSerializer::Save("scene.forge", m_Scene, m_State.World))
            EditorUI::PushLog(m_State, "[SAVE] Scene saved to 'scene.forge'.");
        else
            EditorUI::PushLog(m_State, "[ERROR] Failed to save scene.");
    };
    cb.OnLoadScene = [this]() {
        if (SceneSerializer::Load("scene.forge", m_Scene, m_State.World)) {
            m_State.SelectedIndex = m_Scene.GetEntities().empty() ? -1 : 0;
            EditorUI::PushLog(m_State, "[LOAD] Loaded 'scene.forge'.");
        } else {
            EditorUI::PushLog(m_State, "[ERROR] 'scene.forge' not found. Save first.");
        }
    };
    cb.OnPlay = [this]() {
        if (!m_State.IsPlaying) {
            SceneSerializer::Save(".play_backup.forge", m_Scene, m_State.World);
            m_State.IsPlaying = true;
            m_State.IsPaused  = false;
            EditorUI::PushLog(m_State, "[PLAY] Physics simulation started.");
        } else {
            m_State.IsPaused = false;
        }
    };
    cb.OnPause = [this]() {
        if (m_State.IsPlaying) {
            m_State.IsPaused = !m_State.IsPaused;
            EditorUI::PushLog(m_State, m_State.IsPaused ? "[PAUSE] Simulation paused." : "[PLAY] Simulation resumed.");
        }
    };
    cb.OnStop = [this]() {
        if (m_State.IsPlaying) {
            m_State.IsPlaying = false;
            m_State.IsPaused  = false;
            SceneSerializer::Load(".play_backup.forge", m_Scene, m_State.World);
            EditorUI::PushLog(m_State, "[STOP] Scene restored to pre-play state.");
        }
    };
    cb.OnResetCamera = [this]() {
        ResetCamera();
        EditorUI::PushLog(m_State, "[CAMERA] Camera reset.");
    };
    cb.OnExit = [this]() { Close(); };
    cb.OnAddEntity = [this](const std::string& name, Forge::MeshType type) {
        AddEntity(name, type);
    };

    m_UI.Render(m_Scene, m_Camera, m_State, cb);
}

void EditorApp::OnShutdown() {
    m_Framebuffer.reset();
    m_SceneRenderer.reset();
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
    EditorUI::PushLog(m_State, "[ENTITY] Added: " + name);
}
