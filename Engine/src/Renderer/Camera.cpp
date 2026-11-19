#include "Forge/Renderer/Camera.h"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace Forge {

    Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_Fov(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip) {
        UpdateProjection();
        UpdateVectors();
    }

    void Camera::Rotate(float yawOffset, float pitchOffset) {
        m_Yaw += yawOffset;
        m_Pitch = std::clamp(m_Pitch + pitchOffset, -89.0f, 89.0f);
        UpdateVectors();
    }

    void Camera::SetRotation(float yaw, float pitch) {
        m_Yaw = yaw;
        m_Pitch = std::clamp(pitch, -89.0f, 89.0f);
        UpdateVectors();
    }

    void Camera::SetAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        UpdateProjection();
    }

    glm::mat4 Camera::GetViewMatrix() const {
        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    void Camera::UpdateProjection() {
        m_Projection = glm::perspective(glm::radians(m_Fov), m_AspectRatio, m_NearClip, m_FarClip);
    }

    void Camera::UpdateVectors() {
        glm::vec3 front;
        front.x = std::cos(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));
        front.y = std::sin(glm::radians(m_Pitch));
        front.z = std::sin(glm::radians(m_Yaw)) * std::cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up    = glm::normalize(glm::cross(m_Right, m_Front));
    }

}
