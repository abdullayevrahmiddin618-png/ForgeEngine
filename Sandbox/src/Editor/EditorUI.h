#pragma once
#include <Forge/Forge.h>
#include "SceneSerializer.h"
#include "ViewportGizmo.h"
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

struct EditorState {
    int SelectedIndex          = 1;
    bool IsPlaying             = false;
    bool IsPaused              = false;
    bool ShowGrid              = true;
    GizmoMode ActiveGizmo      = GizmoMode::Translate;
    float LastDt               = 0.0f;
    float CameraSpeed          = 4.0f;
    float MouseSensitivity     = 0.15f;
    uint32_t ViewportTextureID = 0;
    glm::vec2 ViewportSize{ 800.0f, 450.0f };
    bool ViewportHovered       = false;
    WorldSettings World;
    std::vector<std::string> Logs;
};

struct EditorCallbacks {
    std::function<void()> OnNewScene;
    std::function<void()> OnSaveScene;
    std::function<void()> OnLoadScene;
    std::function<void()> OnPlay;
    std::function<void()> OnPause;
    std::function<void()> OnStop;
    std::function<void()> OnResetCamera;
    std::function<void()> OnExit;
    std::function<void(const std::string&, Forge::MeshType)> OnAddEntity;
};

class EditorUI {
public:
    void Render(Forge::Scene& scene, const Forge::Camera& camera, EditorState& state, const EditorCallbacks& cb);
    static void PushLog(EditorState& state, const std::string& msg);

private:
    void ApplyForgeTheme();
    static bool MatchesFilter(const std::string& text, const char* filter);

    ViewportGizmo m_Gizmo;
    bool m_ThemeApplied    = false;
    char m_SearchBuf[64]   = "";
    char m_ObjPathBuf[256] = "Assets/model.obj";
    char m_TexPathBuf[256] = "Assets/texture.png";
};
