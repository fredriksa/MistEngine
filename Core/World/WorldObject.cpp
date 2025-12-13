#include "WorldObject.h"

#include "../Components/Component.h"

#include <ranges>

namespace Core
{
    WorldObject::WorldObject(std::shared_ptr<EngineContext> Context)
        : Context(std::move(Context))
    {
    }

    void WorldObject::Tick(float DeltaTimeS)
    {
        for (std::shared_ptr<Component>& Component : TypeToComponent | std::views::values)
        {
            Component->Tick(DeltaTimeS);
        }
    }

    void WorldObject::Render()
    {
        for (std::shared_ptr<Component>& Component : TypeToComponent | std::views::values)
        {
            Component->Render();
        }
    }

    void WorldObject::AttachComponent(std::shared_ptr<Component> Component)
    {
        TypeToComponent.emplace(std::type_index(typeid(*Component)), Component);
    }

    const EngineContext& WorldObject::GetContext() const
    {
        return *Context;
    }
}
