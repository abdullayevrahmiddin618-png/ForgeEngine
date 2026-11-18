#include "Forge/Renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Forge {

    Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_Fov(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip) {
        UpdateProjection();
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

}
