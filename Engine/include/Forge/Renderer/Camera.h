#pragma once

#include <glm/glm.hpp>

namespace Forge {

    class Camera {
    public:
        Camera(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f, float nearClip = 0.1f, float farClip = 100.0f);

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        const glm::vec3& GetPosition() const { return m_Position; }

        void Move(const glm::vec3& offset) { m_Position += offset; }
        void SetAspectRatio(float aspectRatio);

        glm::mat4 GetViewMatrix() const;
        glm::mat4 GetProjectionMatrix() const { return m_Projection; }
        glm::mat4 GetViewProjectionMatrix() const { return m_Projection * GetViewMatrix(); }

    private:
        void UpdateProjection();

        float m_Fov;
        float m_AspectRatio;
        float m_NearClip;
        float m_FarClip;

        glm::vec3 m_Position{ 0.0f, 0.0f, 3.0f };
        glm::vec3 m_Front{ 0.0f, 0.0f, -1.0f };
        glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };

        glm::mat4 m_Projection{ 1.0f };
    };

}
