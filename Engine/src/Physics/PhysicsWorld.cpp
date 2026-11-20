#include "Forge/Physics/PhysicsWorld.h"
#include <algorithm>
#include <cmath>

namespace Forge {

    namespace {
        glm::vec3 GetHalfExtents(const Entity& e) {
            glm::vec3 half = glm::abs(e.TransformData.Scale) * 0.5f;
            if (e.Shape == MeshType::Plane) {
                half.y = 0.05f;
            }
            return half;
        }
    }

    void PhysicsWorld::Step(Scene& scene, float dt, float gravity) {
        // Kadr sekinlashsa obyektlar devordan o'tib ketmasligi uchun dt ni cheklaymiz
        float stepDt = std::min(dt, 0.05f);
        auto& entities = scene.GetEntities();

        // 1. Gravitatsiya va harakat (Dynamic obyektlar uchun)
        for (auto& e : entities) {
            if (!e.Visible || !e.PhysicsEnabled || e.IsStatic) continue;
            e.Velocity.y += gravity * stepDt;
            e.TransformData.Position += e.Velocity * stepDt;
        }

        // 2. AABB To'qnashuvlarni (Collision) tekshirish va hal qilish
        for (size_t i = 0; i < entities.size(); ++i) {
            auto& a = entities[i];
            if (!a.Visible || !a.PhysicsEnabled) continue;

            for (size_t j = i + 1; j < entities.size(); ++j) {
                auto& b = entities[j];
                if (!b.Visible || !b.PhysicsEnabled) continue;
                if (a.IsStatic && b.IsStatic) continue;

                glm::vec3 halfA = GetHalfExtents(a);
                glm::vec3 halfB = GetHalfExtents(b);
                glm::vec3 delta = b.TransformData.Position - a.TransformData.Position;

                float overlapX = (halfA.x + halfB.x) - std::abs(delta.x);
                float overlapY = (halfA.y + halfB.y) - std::abs(delta.y);
                float overlapZ = (halfA.z + halfB.z) - std::abs(delta.z);

                if (overlapX <= 0.0f || overlapY <= 0.0f || overlapZ <= 0.0f) {
                    continue;
                }

                // Eng kichik kirish o'qini topamiz (Minimum Translation Vector)
                glm::vec3 normal(0.0f);
                float penetration = overlapY;
                if (overlapX < overlapY && overlapX < overlapZ) {
                    penetration = overlapX;
                    normal.x = (delta.x < 0.0f) ? -1.0f : 1.0f;
                } else if (overlapZ < overlapY && overlapZ < overlapX) {
                    penetration = overlapZ;
                    normal.z = (delta.z < 0.0f) ? -1.0f : 1.0f;
                } else {
                    penetration = overlapY;
                    normal.y = (delta.y < 0.0f) ? -1.0f : 1.0f;
                }

                // Obyektlarni bir-birining ichidan chiqarish
                if (a.IsStatic) {
                    b.TransformData.Position += normal * penetration;
                } else if (b.IsStatic) {
                    a.TransformData.Position -= normal * penetration;
                } else {
                    a.TransformData.Position -= normal * (penetration * 0.5f);
                    b.TransformData.Position += normal * (penetration * 0.5f);
                }

                // Sakrash (Restitution / Bounce)
                glm::vec3 relVel = b.Velocity - a.Velocity;
                float velAlongNormal = glm::dot(relVel, normal);
                if (velAlongNormal > 0.0f) continue;

                float eRest = std::min(a.Bounciness, b.Bounciness);
                if (std::abs(velAlongNormal) < 0.4f) {
                    eRest = 0.0f; // Juda sekin urilganda titrab qolmasligi uchun
                }

                float jImpulse = -(1.0f + eRest) * velAlongNormal;
                if (a.IsStatic) {
                    b.Velocity += normal * jImpulse;
                    if (normal.y > 0.5f) {
                        b.Velocity.x *= 0.92f;
                        b.Velocity.z *= 0.92f;
                    }
                } else if (b.IsStatic) {
                    a.Velocity -= normal * jImpulse;
                    if (normal.y < -0.5f) {
                        a.Velocity.x *= 0.92f;
                        a.Velocity.z *= 0.92f;
                    }
                } else {
                    a.Velocity -= normal * (jImpulse * 0.5f);
                    b.Velocity += normal * (jImpulse * 0.5f);
                }
            }
        }
    }

}
