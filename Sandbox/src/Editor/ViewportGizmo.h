#pragma once
#include <Forge/Forge.h>
#include <glm/glm.hpp>

enum class GizmoMode {
    Translate = 0,
    Rotate    = 1,
    Scale     = 2
};

class ViewportGizmo {
public:
    void UpdateAndDraw(
        Forge::Scene& scene,
        const Forge::Camera& camera,
        int& selectedIndex,
        GizmoMode mode,
        const glm::vec2& viewportPos,
        const glm::vec2& viewportSize,
        bool viewportHovered
    );

    bool IsUsing() const { return m_ActiveAxis != -1; }

private:
    bool WorldToScreen(
        const glm::vec3& worldPos,
        const glm::mat4& viewProj,
        const glm::vec2& vpPos,
        const glm::vec2& vpSize,
        glm::vec2& outScreen
    ) const;

    float DistToSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) const;
    int PickEntity(
        Forge::Scene& scene,
        const Forge::Camera& camera,
        const glm::vec2& mousePos,
        const glm::vec2& vpPos,
        const glm::vec2& vpSize
    ) const;

    int m_ActiveAxis = -1; // -1: yo'q, 0: X, 1: Y, 2: Z
};
