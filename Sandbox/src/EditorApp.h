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
    const Forge::VertexArray& GetMesh(Forge::MeshType type) const;

    Forge::Camera m_Camera;
    Forge::Scene m_Scene;
    EditorUI m_UI;
    EditorState m_State;

    std::unique_ptr<Forge::Shader> m_Shader;
    std::unique_ptr<Forge::Texture> m_Texture;
    std::unique_ptr<Forge::VertexArray> m_CubeMesh;
    std::unique_ptr<Forge::VertexArray> m_SphereMesh;
    std::unique_ptr<Forge::VertexArray> m_PyramidMesh;
    std::unique_ptr<Forge::VertexArray> m_PlaneMesh;
    std::unique_ptr<Forge::Framebuffer> m_Framebuffer;
    std::unique_ptr<Forge::Grid> m_Grid;

    bool m_IsLooking   = false;
    float m_LastMouseX = 0.0f;
    float m_LastMouseY = 0.0f;
};
