#include "ComponentManager.h"

#include "../Components/Component.h"
#include <ranges>

namespace Core
{
    ComponentManager::ComponentManager(WorldObject* Owner)
        : Owner(Owner)
    {
    }

    void ComponentManager::Attach(const std::shared_ptr<Component>& Component)
    {
        TypeToComponent.emplace(std::type_index(typeid(*Component)), Component);
    }

    bool ComponentManager::Remove(Component* Comp)
    {
        if (!Comp)
        {
            return false;
        }

        auto it = TypeToComponent.find(std::type_index(typeid(*Comp)));
        if (it != TypeToComponent.end())
        {
            TypeToComponent.erase(it);
            return true;
        }

        return false;
    }

    void ComponentManager::Tick(float DeltaTimeS)
    {
        for (std::shared_ptr<Component>& Component : TypeToComponent | std::views::values)
        {
            Component->Tick(DeltaTimeS);
        }
    }

    void ComponentManager::Render()
    {
        for (std::shared_ptr<Component>& Component : TypeToComponent | std::views::values)
        {
            Component->Render();
        }
    }

    void ComponentManager::Start()
    {
        for (std::shared_ptr<Component>& Component : TypeToComponent | std::views::values)
        {
            Component->Start();
        }
    }

    void ComponentManager::Shutdown()
    {
        for (std::shared_ptr<Component>& Comp : TypeToComponent | std::views::values)
        {
            if (Comp)
            {
                Comp->Shutdown();
            }
        }
    }
}
