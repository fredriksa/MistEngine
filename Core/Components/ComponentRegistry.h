#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "Component.h"

namespace Core
{
    class WorldObject;

    class ComponentRegistry
    {
    public:
        using FactoryFunc = std::function<std::unique_ptr<Component>(const std::shared_ptr<WorldObject>&)>;

        static ComponentRegistry& Get();

        void Register(const std::string& Name, FactoryFunc Factory);
        std::unique_ptr<Component> Create(const std::string& Name, const std::shared_ptr<WorldObject>& Owner);

    private:
        ComponentRegistry() = default;
        std::unordered_map<std::string, FactoryFunc> Factories;
    };

#define REGISTER_COMPONENT(ClassName) \
    namespace { \
        static bool ClassName##_registered = []() { \
            Core::ComponentRegistry::Get().Register(#ClassName, \
            [](const std::shared_ptr<Core::WorldObject>& Owner) { \
            return std::make_unique<ClassName>(Owner); \
            }); \
        return true; \
        }(); \
    }
}
