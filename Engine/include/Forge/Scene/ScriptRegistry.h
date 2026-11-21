#pragma once
#include "Forge/Scene/Script.h"
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Forge {

    class ScriptRegistry {
    public:
        using FactoryFn = std::function<std::shared_ptr<Script>()>;

        template<typename T>
        static void Register(const std::string& name) {
            GetFactories()[name] = []() { return std::make_shared<T>(); };
        }

        static std::shared_ptr<Script> Create(const std::string& name) {
            auto& map = GetFactories();
            auto it = map.find(name);
            if (it != map.end()) {
                return it->second();
            }
            return nullptr;
        }

        static std::vector<std::string> GetRegisteredNames() {
            std::vector<std::string> names;
            for (const auto& [name, _] : GetFactories()) {
                names.push_back(name);
            }
            return names;
        }

    private:
        static std::unordered_map<std::string, FactoryFn>& GetFactories() {
            static std::unordered_map<std::string, FactoryFn> factories;
            return factories;
        }
    };

}
