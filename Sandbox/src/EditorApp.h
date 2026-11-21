#pragma once
#include <Forge/Forge.h>
#include "Editor/EditorUI.h"
#include <memory>
#include <string>

class EditorApp : public Forge::Application {
public:
    EditorApp();

protected:
    void OnInit() override;
    void OnUpdate(float dt) override;
    void OnImGuiRender() override;
    void OnShutdown() override;

private:
    void SetupDefaultScene();
    void ResetCamera();
    void AddEntity(const std::string& baseName, Forge::MeshType type);

    Forge::Camera m_Camera;
    Forge::Scene m_Scene;
    EditorUI m_UI;
    EditorState m_State;

    std::unique_ptr<Forge::SceneRenderer> m_SceneRenderer;
    std::unique_ptr<Forge::Framebuffer> m_Framebuffer;

    bool m_IsLooking   = false;
    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;
};
