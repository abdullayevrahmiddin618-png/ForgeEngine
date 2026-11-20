#pragma once
#include <Forge/Forge.h>
#include "SceneSerializer.h"
#include <functional>
#include <string>
#include <vector>

struct EditorState {
    int SelectedIndex      = 1;
    bool IsPlaying         = false;
    bool IsPaused          = false;
    float LastDt           = 0.0f;
    float CameraSpeed      = 4.0f;
    float MouseSensitivity = 0.15f;
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
    void Render(Forge::Scene& scene, EditorState& state, const EditorCallbacks& cb);
    static void PushLog(EditorState& state, const std::string& msg);

private:
    void ApplyForgeTheme();
    static bool MatchesFilter(const std::string& text, const char* filter);

    bool m_ThemeApplied = false;
    char m_SearchBuf[64] = "";
};
