#include "ViewportGizmo.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

bool ViewportGizmo::WorldToScreen(
    const glm::vec3& worldPos,
    const glm::mat4& viewProj,
    const glm::vec2& vpPos,
    const glm::vec2& vpSize,
    glm::vec2& outScreen
) const {
    glm::vec4 clip = viewProj * glm::vec4(worldPos, 1.0f);
    if (clip.w <= 0.001f) return false;

    glm::vec3 ndc = glm::vec3(clip) / clip.w;
    outScreen.x = vpPos.x + (ndc.x * 0.5f + 0.5f) * vpSize.x;
    outScreen.y = vpPos.y + (1.0f - (ndc.y * 0.5f + 0.5f)) * vpSize.y;
    return true;
}

float ViewportGizmo::DistToSegment(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) const {
    glm::vec2 ab = b - a;
    float lenSq = glm::dot(ab, ab);
    if (lenSq < 0.0001f) return glm::length(p - a);
    float t = std::clamp(glm::dot(p - a, ab) / lenSq, 0.0f, 1.0f);
    return glm::length(p - (a + t * ab));
}

int ViewportGizmo::PickEntity(
    Forge::Scene& scene,
    const Forge::Camera& camera,
    const glm::vec2& mousePos,
    const glm::vec2& vpPos,
    const glm::vec2& vpSize
) const {
    if (vpSize.x <= 0.0f || vpSize.y <= 0.0f) return -1;

    float ndcX = ((mousePos.x - vpPos.x) / vpSize.x) * 2.0f - 1.0f;
    float ndcY = 1.0f - ((mousePos.y - vpPos.y) / vpSize.y) * 2.0f;

    glm::mat4 invVP = glm::inverse(camera.GetViewProjectionMatrix());
    glm::vec4 nearW = invVP * glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 farW  = invVP * glm::vec4(ndcX, ndcY,  1.0f, 1.0f);
    nearW /= nearW.w;
    farW  /= farW.w;

    glm::vec3 rayOrigin = glm::vec3(nearW);
    glm::vec3 rayDir    = glm::normalize(glm::vec3(farW - nearW));

    int hitIndex = -1;
    float closestT = std::numeric_limits<float>::max();

    const auto& entities = scene.GetEntities();
    for (int i = 0; i < static_cast<int>(entities.size()); ++i) {
        const auto& e = entities[i];
        if (!e.Visible) continue;

        glm::vec3 halfExt = glm::abs(e.TransformData.Scale) * 0.5f;
        if (e.Shape == Forge::MeshType::Plane) {
            halfExt.y = 0.06f;
        }
        glm::vec3 bMin = e.TransformData.Position - halfExt;
        glm::vec3 bMax = e.TransformData.Position + halfExt;

        float tMin = 0.0f;
        float tMax = closestT;
        bool hit = true;

        for (int axis = 0; axis < 3; ++axis) {
            if (std::abs(rayDir[axis]) < 1e-6f) {
                if (rayOrigin[axis] < bMin[axis] || rayOrigin[axis] > bMax[axis]) {
                    hit = false;
                    break;
                }
            } else {
                float invD = 1.0f / rayDir[axis];
                float t1 = (bMin[axis] - rayOrigin[axis]) * invD;
                float t2 = (bMax[axis] - rayOrigin[axis]) * invD;
                if (t1 > t2) std::swap(t1, t2);
                tMin = std::max(tMin, t1);
                tMax = std::min(tMax, t2);
                if (tMin > tMax) {
                    hit = false;
                    break;
                }
            }
        }

        if (hit && tMin < closestT) {
            closestT = tMin;
            hitIndex = i;
        }
    }

    return hitIndex;
}

void ViewportGizmo::UpdateAndDraw(
    Forge::Scene& scene,
    const Forge::Camera& camera,
    int& selectedIndex,
    GizmoMode mode,
    const glm::vec2& viewportPos,
    const glm::vec2& viewportSize,
    bool viewportHovered
) {
    ImVec2 mp = ImGui::GetMousePos();
    glm::vec2 mousePos{ mp.x, mp.y };

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        m_ActiveAxis = -1;
    }

    auto& entities = scene.GetEntities();
    bool hasSelection = (selectedIndex >= 0 && selectedIndex < static_cast<int>(entities.size()));
    int hoveredAxis = -1;

    if (hasSelection) {
        auto& e = entities[selectedIndex];
        glm::vec3 origin = e.TransformData.Position;
        glm::mat4 viewProj = camera.GetViewProjectionMatrix();
        ImDrawList* draw = ImGui::GetWindowDrawList();

        float distToCam = glm::length(camera.GetPosition() - origin);
        float axisLen = std::max(0.6f, distToCam * 0.18f);

        const glm::vec3 axisDirs[3] = {
            { 1.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f }
        };
        const ImU32 axisColors[3] = {
            IM_COL32(235, 65, 65, 255),  // X - Qizil
            IM_COL32(65, 220, 95, 255),  // Y - Yashil
            IM_COL32(65, 145, 245, 255)  // Z - Ko'k
        };
        const char* axisLabels[3] = { "X", "Y", "Z" };

        if (mode == GizmoMode::Rotate) {
            // ROTATE: 3D halqalar (aylanalar)
            constexpr int kSegments = 36;
            constexpr float kTwoPi = 6.2831853f;

            const glm::vec3 basisU[3] = { {0, 1, 0}, {1, 0, 0}, {1, 0, 0} };
            const glm::vec3 basisV[3] = { {0, 0, 1}, {0, 0, 1}, {0, 1, 0} };

            std::vector<glm::vec2> ringPts[3];
            for (int axis = 0; axis < 3; ++axis) {
                ringPts[axis].reserve(kSegments + 1);
                for (int s = 0; s <= kSegments; ++s) {
                    float a = (static_cast<float>(s) / kSegments) * kTwoPi;
                    glm::vec3 p3 = origin + (basisU[axis] * std::cos(a) + basisV[axis] * std::sin(a)) * axisLen;
                    glm::vec2 p2;
                    if (WorldToScreen(p3, viewProj, viewportPos, viewportSize, p2)) {
                        ringPts[axis].push_back(p2);
                    }
                }

                if (viewportHovered && m_ActiveAxis == -1 && ringPts[axis].size() > 1) {
                    for (size_t s = 0; s + 1 < ringPts[axis].size(); ++s) {
                        if (DistToSegment(mousePos, ringPts[axis][s], ringPts[axis][s + 1]) < 9.0f) {
                            hoveredAxis = axis;
                            break;
                        }
                    }
                }
            }

            if (hoveredAxis != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_ActiveAxis = hoveredAxis;
            }

            if (m_ActiveAxis != -1 && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                ImVec2 delta = ImGui::GetIO().MouseDelta;
                e.TransformData.Rotation[m_ActiveAxis] += (delta.x - delta.y) * 0.65f;
            }

            for (int axis = 0; axis < 3; ++axis) {
                if (ringPts[axis].size() < 2) continue;
                bool active = (m_ActiveAxis == axis || hoveredAxis == axis);
                ImU32 col = active ? IM_COL32(255, 220, 60, 255) : axisColors[axis];
                float thickness = active ? 3.8f : 2.2f;

                for (size_t s = 0; s + 1 < ringPts[axis].size(); ++s) {
                    draw->AddLine(
                        ImVec2(ringPts[axis][s].x, ringPts[axis][s].y),
                        ImVec2(ringPts[axis][s + 1].x, ringPts[axis][s + 1].y),
                        col, thickness
                    );
                }
            }
        } else {
            // MOVE (strelkalar) va SCALE (kubiklar)
            glm::vec2 origin2D;
            if (WorldToScreen(origin, viewProj, viewportPos, viewportSize, origin2D)) {
                glm::vec2 tip2D[3];
                bool tipValid[3] = { false, false, false };

                for (int i = 0; i < 3; ++i) {
                    tipValid[i] = WorldToScreen(origin + axisDirs[i] * axisLen, viewProj, viewportPos, viewportSize, tip2D[i]);
                    if (tipValid[i] && viewportHovered && m_ActiveAxis == -1) {
                        if (DistToSegment(mousePos, origin2D, tip2D[i]) < 11.0f) {
                            hoveredAxis = i;
                        }
                    }
                }

                if (hoveredAxis != -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    m_ActiveAxis = hoveredAxis;
                }

                if (m_ActiveAxis != -1 && ImGui::IsMouseDown(ImGuiMouseButton_Left) && tipValid[m_ActiveAxis]) {
                    ImVec2 delta = ImGui::GetIO().MouseDelta;
                    glm::vec2 mouseDelta{ delta.x, delta.y };
                    glm::vec2 screenAxis = tip2D[m_ActiveAxis] - origin2D;
                    float screenLen = glm::length(screenAxis);

                    if (screenLen > 1.0f) {
                        float amount = glm::dot(mouseDelta, screenAxis / screenLen);
                        float worldPerPixel = axisLen / screenLen;

                        if (mode == GizmoMode::Translate) {
                            e.TransformData.Position += axisDirs[m_ActiveAxis] * (amount * worldPerPixel);
                        } else if (mode == GizmoMode::Scale) {
                            e.TransformData.Scale += axisDirs[m_ActiveAxis] * (amount * worldPerPixel);
                            e.TransformData.Scale = glm::max(e.TransformData.Scale, glm::vec3(0.05f));
                        }
                    }
                }

                for (int i = 0; i < 3; ++i) {
                    if (!tipValid[i]) continue;
                    bool active = (m_ActiveAxis == i || hoveredAxis == i);
                    ImU32 col = active ? IM_COL32(255, 220, 60, 255) : axisColors[i];
                    float thickness = active ? 4.0f : 2.8f;

                    draw->AddLine(ImVec2(origin2D.x, origin2D.y), ImVec2(tip2D[i].x, tip2D[i].y), col, thickness);

                    if (mode == GizmoMode::Scale) {
                        float r = active ? 6.0f : 4.5f;
                        draw->AddRectFilled(
                            ImVec2(tip2D[i].x - r, tip2D[i].y - r),
                            ImVec2(tip2D[i].x + r, tip2D[i].y + r),
                            col
                        );
                    } else {
                        // Move: uchburchak strelka
                        glm::vec2 dir = tip2D[i] - origin2D;
                        float len = glm::length(dir);
                        if (len > 1.0f) {
                            dir /= len;
                            glm::vec2 perp(-dir.y, dir.x);
                            glm::vec2 base = tip2D[i] - dir * 12.0f;
                            draw->AddTriangleFilled(
                                ImVec2(tip2D[i].x + dir.x * 4.0f, tip2D[i].y + dir.y * 4.0f),
                                ImVec2(base.x + perp.x * 5.5f, base.y + perp.y * 5.5f),
                                ImVec2(base.x - perp.x * 5.5f, base.y - perp.y * 5.5f),
                                col
                            );
                        }
                    }

                    draw->AddText(ImVec2(tip2D[i].x + 8.0f, tip2D[i].y - 7.0f), col, axisLabels[i]);
                }
                draw->AddCircleFilled(ImVec2(origin2D.x, origin2D.y), 4.5f, IM_COL32(240, 240, 240, 255));
            }
        }
    }

    // Agar Gizmo bosilmagan bo'lsa, sichqoncha bilan obyektni tanlash (Raycasting)
    if (viewportHovered && hoveredAxis == -1 && m_ActiveAxis == -1 && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        selectedIndex = PickEntity(scene, camera, mousePos, viewportPos, viewportSize);
    }
}
