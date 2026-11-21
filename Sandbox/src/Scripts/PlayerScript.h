#pragma once
#include <Forge/Forge.h>
#include <cmath>

class PlayerScript : public Forge::Script {
public:
    float MoveSpeed = 5.5f;
    float JumpForce = 6.2f;

    void OnCreate() override {
        if (m_Entity) {
            m_Entity->PhysicsEnabled = true;
            m_Entity->IsStatic       = false;
        }
    }

    void OnUpdate(float dt) override {
        if (!m_Entity) return;

        // Horizontal movement (WASD)
        glm::vec3 moveDir(0.0f);
        if (Forge::Input::IsKeyPressed(Forge::Key::W)) moveDir.z -= 1.0f;
        if (Forge::Input::IsKeyPressed(Forge::Key::S)) moveDir.z += 1.0f;
        if (Forge::Input::IsKeyPressed(Forge::Key::A)) moveDir.x -= 1.0f;
        if (Forge::Input::IsKeyPressed(Forge::Key::D)) moveDir.x += 1.0f;

        if (glm::length(moveDir) > 0.01f) {
            moveDir = glm::normalize(moveDir);
            m_Entity->Velocity.x = moveDir.x * MoveSpeed;
            m_Entity->Velocity.z = moveDir.z * MoveSpeed;
        } else {
            m_Entity->Velocity.x *= 0.85f;
            m_Entity->Velocity.z *= 0.85f;
        }

        // Jump (Space) when nearly grounded
        bool grounded = std::abs(m_Entity->Velocity.y) < 0.08f;
        if (grounded && ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
            m_Entity->Velocity.y = JumpForce;
        }

        // Smooth third-person camera follow
        if (m_Camera) {
            glm::vec3 targetPos = m_Entity->TransformData.Position + glm::vec3(0.0f, 3.0f, 6.0f);
            glm::vec3 current   = m_Camera->GetPosition();
            float smooth        = std::min(1.0f, dt * 6.0f);
            m_Camera->SetPosition(glm::mix(current, targetPos, smooth));
            m_Camera->SetRotation(-90.0f, -20.0f);
        }
    }
};
