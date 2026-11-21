#include "EditorUI.h"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>

void EditorUI::PushLog(EditorState& state, const std::string& msg) {
    state.Logs.push_back(msg);
    if (state.Logs.size() > 150) {
        state.Logs.erase(state.Logs.begin());
    }
}

bool EditorUI::MatchesFilter(const std::string& text, const char* filter) {
    if (!filter || filter[0] == '\0') return true;
    std::string a = text, b = filter;
    std::transform(a.begin(), a.end(), a.begin(), ::tolower);
    std::transform(b.begin(), b.end(), b.begin(), ::tolower);
    return a.find(b) != std::string::npos;
}

void EditorUI::ApplyForgeTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding   = 0.0f;
    s.FrameRounding    = 4.0f;
    s.GrabRounding     = 4.0f;
    s.TabRounding      = 4.0f;
    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize  = 0.0f;
    s.WindowPadding    = ImVec2(10.0f, 8.0f);
    s.FramePadding     = ImVec2(6.0f, 4.0f);
    s.ItemSpacing      = ImVec2(6.0f, 5.0f);

    auto& c = s.Colors;
    c[ImGuiCol_WindowBg]         = ImVec4(0.08f, 0.09f, 0.11f, 0.96f);
    c[ImGuiCol_ChildBg]          = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);
    c[ImGuiCol_PopupBg]          = ImVec4(0.10f, 0.11f, 0.14f, 0.98f);
    c[ImGuiCol_Border]           = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    c[ImGuiCol_TitleBg]          = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);
    c[ImGuiCol_TitleBgActive]    = ImVec4(0.09f, 0.11f, 0.14f, 1.00f);
    c[ImGuiCol_MenuBarBg]        = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);

    c[ImGuiCol_Header]           = ImVec4(0.10f, 0.42f, 0.58f, 0.85f);
    c[ImGuiCol_HeaderHovered]    = ImVec4(0.14f, 0.54f, 0.72f, 0.90f);
    c[ImGuiCol_HeaderActive]     = ImVec4(0.16f, 0.64f, 0.85f, 1.00f);

    c[ImGuiCol_Button]           = ImVec4(0.14f, 0.17f, 0.22f, 1.00f);
    c[ImGuiCol_ButtonHovered]    = ImVec4(0.16f, 0.48f, 0.65f, 1.00f);
    c[ImGuiCol_ButtonActive]     = ImVec4(0.18f, 0.62f, 0.82f, 1.00f);

    c[ImGuiCol_FrameBg]          = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
    c[ImGuiCol_FrameBgHovered]   = ImVec4(0.16f, 0.19f, 0.24f, 1.00f);
    c[ImGuiCol_FrameBgActive]    = ImVec4(0.20f, 0.24f, 0.30f, 1.00f);

    c[ImGuiCol_Tab]              = ImVec4(0.10f, 0.12f, 0.15f, 1.00f);
    c[ImGuiCol_TabHovered]       = ImVec4(0.16f, 0.48f, 0.65f, 1.00f);
    c[ImGuiCol_TabActive]        = ImVec4(0.12f, 0.44f, 0.62f, 1.00f);

    c[ImGuiCol_CheckMark]        = ImVec4(0.24f, 0.78f, 0.96f, 1.00f);
    c[ImGuiCol_SliderGrab]       = ImVec4(0.20f, 0.66f, 0.86f, 1.00f);
    c[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.80f, 1.00f, 1.00f);
}

void EditorUI::HandleHotkeys(Forge::Scene& scene, EditorState& state, const EditorCallbacks& cb) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantTextInput || ImGui::IsMouseDown(ImGuiMouseButton_Right) || state.IsPlaying) return;

    // Gizmo shortcuts: 1 = Move, 2 = Rotate, 3 = Scale
    if (ImGui::IsKeyPressed(ImGuiKey_1, false)) state.ActiveGizmo = GizmoMode::Translate;
    if (ImGui::IsKeyPressed(ImGuiKey_2, false)) state.ActiveGizmo = GizmoMode::Rotate;
    if (ImGui::IsKeyPressed(ImGuiKey_3, false)) state.ActiveGizmo = GizmoMode::Scale;

    // Ctrl + S: Save Scene
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S, false)) {
        cb.OnSaveScene();
    }

    auto& entities = scene.GetEntities();
    bool hasSelection = (state.SelectedIndex >= 0 && state.SelectedIndex < static_cast<int>(entities.size()));
    if (!hasSelection) return;

    // F: Focus Camera on Selected Entity
    if (ImGui::IsKeyPressed(ImGuiKey_F, false) && cb.OnFocusEntity) {
        cb.OnFocusEntity();
    }

    // Ctrl + D: Duplicate Selected Entity
    if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D, false)) {
        auto copy = entities[state.SelectedIndex];
        copy.Name += "_Copy";
        copy.TransformData.Position.x += 1.0f;
        copy.ScriptInstance = nullptr;
        auto& ne = scene.CreateEntity(copy.Name);
        ne = copy;
        state.SelectedIndex = static_cast<int>(scene.GetEntities().size()) - 1;
        PushLog(state, "[ENTITY] Duplicated: " + copy.Name);
    }

    // Delete: Remove Selected Entity
    if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
        PushLog(state, "[ENTITY] Deleted: " + entities[state.SelectedIndex].Name);
        scene.RemoveEntity(static_cast<size_t>(state.SelectedIndex));
        state.SelectedIndex = -1;
    }
}

void EditorUI::Render(Forge::Scene& scene, const Forge::Camera& camera, EditorState& state, const EditorCallbacks& cb) {
    if (!m_ThemeApplied) {
        ApplyForgeTheme();
        m_ThemeApplied = true;
    }

    HandleHotkeys(scene, state, cb);

    const ImGuiViewport* vp = ImGui::GetMainViewport();
    float menuH   = 0.0f;
    float barH    = 38.0f;
    float leftW   = 250.0f;
    float rightW  = 300.0f;
    float bottomH = 175.0f;

    // 1. Top Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        menuH = ImGui::GetWindowHeight();

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Scene"))               cb.OnNewScene();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S"))    cb.OnSaveScene();
            if (ImGui::MenuItem("Load Scene"))              cb.OnLoadScene();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ESC"))             cb.OnExit();
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Entity")) {
            if (ImGui::MenuItem("Cube"))    cb.OnAddEntity("Cube", Forge::MeshType::Cube);
            if (ImGui::MenuItem("Sphere"))  cb.OnAddEntity("Sphere", Forge::MeshType::Sphere);
            if (ImGui::MenuItem("Pyramid")) cb.OnAddEntity("Pyramid", Forge::MeshType::Pyramid);
            if (ImGui::MenuItem("Plane"))   cb.OnAddEntity("Plane", Forge::MeshType::Plane);
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Focus Selected", "F")) {
                if (cb.OnFocusEntity) cb.OnFocusEntity();
            }
            if (ImGui::MenuItem("Reset Camera")) cb.OnResetCamera();
            ImGui::EndMenu();
        }

        float fps = (state.LastDt > 0.0f) ? (1.0f / state.LastDt) : 0.0f;
        char buf[64];
        std::snprintf(buf, sizeof(buf), "ForgeEngine  |  %.0f FPS (%.1f ms)", fps, state.LastDt * 1000.0f);
        ImGui::SetCursorPosX(vp->Size.x - ImGui::CalcTextSize(buf).x - 14.0f);
        ImGui::TextColored(ImVec4(0.30f, 0.82f, 0.96f, 1.0f), "%s", buf);

        ImGui::EndMainMenuBar();
    }

    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    // 2. Toolbar
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + menuH));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, barH));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 6.0f));
    ImGui::Begin("##Toolbar", nullptr, flags | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
    {
        if (ImGui::Button("Save", ImVec2(58, 24))) cb.OnSaveScene();
        ImGui::SameLine();
        if (ImGui::Button("Load", ImVec2(58, 24))) cb.OnLoadScene();

        ImGui::SameLine(0.0f, 20.0f);

        auto gizmoBtn = [&](const char* label, GizmoMode mode) {
            bool active = (state.ActiveGizmo == mode);
            ImGui::PushStyleColor(ImGuiCol_Button, active
                ? ImVec4(0.14f, 0.54f, 0.72f, 1.0f) : ImVec4(0.14f, 0.17f, 0.22f, 1.0f));
            if (ImGui::Button(label, ImVec2(64, 24))) state.ActiveGizmo = mode;
            ImGui::PopStyleColor();
        };

        gizmoBtn("Move", GizmoMode::Translate);
        ImGui::SameLine();
        gizmoBtn("Rotate", GizmoMode::Rotate);
        ImGui::SameLine();
        gizmoBtn("Scale", GizmoMode::Scale);

        float centerW = 70.0f * 3.0f + 12.0f;
        ImGui::SameLine();
        ImGui::SetCursorPosX((vp->Size.x - centerW) * 0.5f);

        ImGui::PushStyleColor(ImGuiCol_Button, (state.IsPlaying && !state.IsPaused)
            ? ImVec4(0.14f, 0.58f, 0.30f, 1.0f) : ImVec4(0.16f, 0.20f, 0.25f, 1.0f));
        if (ImGui::Button("Play", ImVec2(70, 24))) cb.OnPlay();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (state.IsPlaying && state.IsPaused)
            ? ImVec4(0.75f, 0.52f, 0.12f, 1.0f) : ImVec4(0.16f, 0.20f, 0.25f, 1.0f));
        if (ImGui::Button("Pause", ImVec2(70, 24))) cb.OnPause();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.58f, 0.16f, 0.20f, 1.0f));
        if (ImGui::Button("Stop", ImVec2(70, 24))) cb.OnStop();
        ImGui::PopStyleColor();
    }
    ImGui::End();
    ImGui::PopStyleVar();

    float topY = menuH + barH;
    float midH = vp->Size.y - topY - bottomH;

    // 3. Left Panel: Hierarchy
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + topY));
    ImGui::SetNextWindowSize(ImVec2(leftW, midH));
    ImGui::Begin("Hierarchy", nullptr, flags);
    {
        if (ImGui::Button("+ Cube", ImVec2(72, 0)))    cb.OnAddEntity("Cube", Forge::MeshType::Cube);
        ImGui::SameLine();
        if (ImGui::Button("+ Sphere", ImVec2(72, 0)))  cb.OnAddEntity("Sphere", Forge::MeshType::Sphere);
        ImGui::SameLine();
        if (ImGui::Button("+ Pyr", ImVec2(72, 0)))     cb.OnAddEntity("Pyramid", Forge::MeshType::Pyramid);

        ImGui::SetNextItemWidth(-1);
        ImGui::InputTextWithHint("##Filter", "Search entities...", m_SearchBuf, sizeof(m_SearchBuf));
        ImGui::Separator();

        auto& entities = scene.GetEntities();
        for (int i = 0; i < static_cast<int>(entities.size()); ++i) {
            if (!MatchesFilter(entities[i].Name, m_SearchBuf)) continue;
            ImGui::PushID(i);
            ImGui::Checkbox("##v", &entities[i].Visible);
            ImGui::SameLine();
            if (ImGui::Selectable(entities[i].Name.c_str(), state.SelectedIndex == i)) {
                state.SelectedIndex = i;
            }
            ImGui::PopID();
        }
    }
    ImGui::End();

    // 4. Right Panel: Inspector
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x + vp->Size.x - rightW, vp->Pos.y + topY));
    ImGui::SetNextWindowSize(ImVec2(rightW, midH));
    ImGui::Begin("Inspector", nullptr, flags);
    {
        auto& entities = scene.GetEntities();
        if (state.SelectedIndex >= 0 && state.SelectedIndex < static_cast<int>(entities.size())) {
            auto& e = entities[state.SelectedIndex];
            ImGui::TextColored(ImVec4(0.30f, 0.82f, 0.96f, 1.0f), "%s", e.Name.c_str());
            ImGui::Separator();

            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::DragFloat3("Position", &e.TransformData.Position.x, 0.05f);
                ImGui::DragFloat3("Rotation", &e.TransformData.Rotation.x, 1.0f);
                ImGui::DragFloat3("Scale",    &e.TransformData.Scale.x,    0.05f);
                if (ImGui::SmallButton("Reset Transform")) {
                    e.TransformData.Position = { 0.0f, 0.0f, 0.0f };
                    e.TransformData.Rotation = { 0.0f, 0.0f, 0.0f };
                    e.TransformData.Scale    = { 1.0f, 1.0f, 1.0f };
                }
            }

            if (ImGui::CollapsingHeader("Mesh & Material", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* shapes[] = { "Cube", "Sphere", "Pyramid", "Plane", "Custom (.obj)" };
                int current = static_cast<int>(e.Shape);
                if (ImGui::Combo("Shape", &current, shapes, 5)) {
                    e.Shape = static_cast<Forge::MeshType>(current);
                }
                ImGui::ColorEdit3("Color", &e.Color.x);

                ImGui::Spacing();
                ImGui::TextDisabled("3D Model (.obj):");
                if (ImGui::Button("Browse .OBJ...", ImVec2(-1, 24))) {
                    std::string path = Forge::FileDialog::OpenFile("OBJ Models (*.obj)\0*.obj\0All Files (*.*)\0*.*\0");
                    if (!path.empty()) {
                        e.MeshPath = path;
                        e.Shape    = Forge::MeshType::Custom;
                        std::strncpy(m_ObjPathBuf, path.c_str(), sizeof(m_ObjPathBuf) - 1);
                        PushLog(state, "[ASSET] Loaded OBJ: " + e.MeshPath);
                    }
                }
                if (!e.MeshPath.empty()) {
                    ImGui::TextWrapped("Active: %s", e.MeshPath.c_str());
                }

                ImGui::Spacing();
                ImGui::TextDisabled("Texture (.png / .jpg):");
                if (ImGui::Button("Browse Texture...", ImVec2(180, 24))) {
                    std::string path = Forge::FileDialog::OpenFile("Images (*.png;*.jpg;*.jpeg)\0*.png;*.jpg;*.jpeg\0All Files (*.*)\0*.*\0");
                    if (!path.empty()) {
                        e.TexturePath = path;
                        std::strncpy(m_TexPathBuf, path.c_str(), sizeof(m_TexPathBuf) - 1);
                        PushLog(state, "[ASSET] Loaded Texture: " + e.TexturePath);
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear", ImVec2(-1, 24))) {
                    e.TexturePath.clear();
                }
                if (!e.TexturePath.empty()) {
                    ImGui::TextWrapped("Active: %s", e.TexturePath.c_str());
                }
            }

            if (ImGui::CollapsingHeader("Physics", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Enable Physics", &e.PhysicsEnabled);
                if (e.PhysicsEnabled) {
                    ImGui::Checkbox("Static Body", &e.IsStatic);
                    if (!e.IsStatic) {
                        ImGui::SliderFloat("Bounciness", &e.Bounciness, 0.0f, 0.95f);
                    }
                }
            }

            if (ImGui::CollapsingHeader("C++ Script", ImGuiTreeNodeFlags_DefaultOpen)) {
                const char* preview = e.ScriptName.empty() ? "None" : e.ScriptName.c_str();
                if (ImGui::BeginCombo("Script", preview)) {
                    if (ImGui::Selectable("None", e.ScriptName.empty())) {
                        e.ScriptName.clear();
                        e.ScriptInstance.reset();
                    }
                    for (const auto& name : Forge::ScriptRegistry::GetRegisteredNames()) {
                        bool selected = (e.ScriptName == name);
                        if (ImGui::Selectable(name.c_str(), selected)) {
                            e.ScriptName = name;
                            PushLog(state, "[SCRIPT] Attached '" + name + "' to " + e.Name);
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            if (ImGui::Button("Duplicate (Ctrl+D)", ImVec2(-1, 26))) {
                auto copy = e;
                copy.Name += "_Copy";
                copy.TransformData.Position.x += 1.0f;
                copy.ScriptInstance = nullptr;

                auto& ne = scene.CreateEntity(copy.Name);
                ne = copy;
                state.SelectedIndex = static_cast<int>(scene.GetEntities().size()) - 1;
                PushLog(state, "[ENTITY] Duplicated: " + copy.Name);
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.58f, 0.16f, 0.20f, 1.0f));
            if (ImGui::Button("Delete (Del)", ImVec2(-1, 26))) {
                PushLog(state, "[ENTITY] Deleted: " + e.Name);
                scene.RemoveEntity(static_cast<size_t>(state.SelectedIndex));
                state.SelectedIndex = -1;
            }
            ImGui::PopStyleColor();
        } else {
            ImGui::TextDisabled("Select an entity from the Viewport or Hierarchy.");
        }
    }
    ImGui::End();

    // 5. Center Panel: 3D Viewport
    float vpX = vp->Pos.x + leftW;
    float vpY = vp->Pos.y + topY;
    float vpW = vp->Size.x - leftW - rightW;
    float vpH = midH;

    ImGui::SetNextWindowPos(ImVec2(vpX, vpY));
    ImGui::SetNextWindowSize(ImVec2(vpW, vpH));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr, flags | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    {
        state.ViewportHovered = ImGui::IsWindowHovered();
        ImVec2 imgPos = ImGui::GetCursorScreenPos();
        ImVec2 avail  = ImGui::GetContentRegionAvail();
        state.ViewportSize = { avail.x, avail.y };

        if (state.ViewportTextureID != 0 && avail.x > 0 && avail.y > 0) {
            ImGui::Image(
                (ImTextureID)(uintptr_t)state.ViewportTextureID,
                avail,
                ImVec2(0.0f, 1.0f),
                ImVec2(1.0f, 0.0f)
            );

            if (!state.IsPlaying) {
                m_Gizmo.UpdateAndDraw(
                    scene,
                    camera,
                    state.SelectedIndex,
                    state.ActiveGizmo,
                    { imgPos.x, imgPos.y },
                    { avail.x, avail.y },
                    state.ViewportHovered
                );
            }
        }
    }
    ImGui::End();
    ImGui::PopStyleVar();

    // 6. Bottom Panel: Console & Environment
    ImGui::SetNextWindowPos(ImVec2(vp->Pos.x, vp->Pos.y + vp->Size.y - bottomH));
    ImGui::SetNextWindowSize(ImVec2(vp->Size.x, bottomH));
    ImGui::Begin("##BottomPanel", nullptr, flags | ImGuiWindowFlags_NoTitleBar);
    {
        if (ImGui::BeginTabBar("BottomTabs")) {
            if (ImGui::BeginTabItem("Console")) {
                if (ImGui::SmallButton("Clear")) state.Logs.clear();
                ImGui::Separator();
                ImGui::BeginChild("LogScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
                for (const auto& line : state.Logs) {
                    ImGui::TextUnformatted(line.c_str());
                }
                if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                    ImGui::SetScrollHereY(1.0f);
                }
                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Environment & Camera")) {
                ImGui::Columns(2, nullptr, false);
                ImGui::DragFloat3("Light Position", &state.World.LightPos.x, 0.1f);
                ImGui::ColorEdit3("Light Color", &state.World.LightColor.x);
                ImGui::SliderFloat("Ambient", &state.World.AmbientStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Specular", &state.World.SpecularStrength, 0.0f, 2.0f);

                ImGui::NextColumn();
                ImGui::ColorEdit3("Sky Color", &state.World.ClearColor.x);
                ImGui::SliderFloat("Camera Speed", &state.CameraSpeed, 1.0f, 15.0f);
                ImGui::SliderFloat("Sensitivity", &state.MouseSensitivity, 0.05f, 0.5f);
                ImGui::Checkbox("Checker Texture", &state.World.UseTexture);
                ImGui::SameLine();
                ImGui::Checkbox("3D Grid", &state.ShowGrid);
                ImGui::Checkbox("Real-Time Shadows", &state.World.EnableShadows);
                ImGui::SameLine();
                ImGui::Checkbox("360 Skybox", &state.World.EnableSkybox);
                ImGui::Columns(1);
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}
