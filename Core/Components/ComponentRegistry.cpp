#include "ComponentRegistry.h"

#include <cstdio>
#include <utility>

namespace Core
{
    ComponentRegistry& ComponentRegistry::Get()
    {
        static ComponentRegistry Instance;
        return Instance;
    }

    void ComponentRegistry::Register(const std::string& Name, FactoryFunc Factory)
    {
        Factories[Name] = std::move(Factory);
        std::printf("Registered component: %s\n", Name.c_str());
    }

    std::shared_ptr<Component> ComponentRegistry::Create(const std::string& Name,
                                                         const std::shared_ptr<WorldObject>& Owner)
    {
        auto It = Factories.find(Name);
        if (It != Factories.end())
        {
            return It->second(Owner);
        }

        std::printf("Warning: Unknown component type '%s'\n", Name.c_str());
        return nullptr;
    }
}
