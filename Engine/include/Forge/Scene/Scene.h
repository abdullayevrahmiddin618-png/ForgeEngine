#pragma once

#include "Forge/Scene/Entity.h"
#include <vector>
#include <string>

namespace Forge {

    class Scene {
    public:
        Entity& CreateEntity(const std::string& name = "Cube") {
            m_Entities.push_back(Entity{ name });
            return m_Entities.back();
        }

        void RemoveEntity(size_t index) {
            if (index < m_Entities.size()) {
                m_Entities.erase(m_Entities.begin() + static_cast<std::ptrdiff_t>(index));
            }
        }

        std::vector<Entity>& GetEntities() { return m_Entities; }
        const std::vector<Entity>& GetEntities() const { return m_Entities; }

    private:
        std::vector<Entity> m_Entities;
    };

}
