#pragma once

namespace Forge {

    struct Entity;
    class Scene;
    class Camera;

    class Script {
    public:
        virtual ~Script() = default;

        virtual void OnCreate() {}
        virtual void OnUpdate(float dt) {}
        virtual void OnDestroy() {}

        void Bind(Entity* entity, Scene* scene, Camera* camera) {
            m_Entity = entity;
            m_Scene  = scene;
            m_Camera = camera;
        }

        Entity* GetEntity() const { return m_Entity; }
        Scene* GetScene() const { return m_Scene; }
        Camera* GetCamera() const { return m_Camera; }

    protected:
        Entity* m_Entity = nullptr;
        Scene* m_Scene   = nullptr;
        Camera* m_Camera = nullptr;
    };

}
