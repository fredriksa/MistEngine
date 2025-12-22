#include "WorldObject.h"
#include "World.h"
#include "../Components/TransformComponent.h"

namespace Core
{
    WorldObject::WorldObject(std::shared_ptr<EngineContext> Context, World* InWorld)
        : Context(std::move(Context))
          , OwningWorld(InWorld)
    {
    }

    void WorldObject::SetName(const std::string& InName)
    {
        if (OwningWorld && !Name.empty())
        {
            OwningWorld->UnregisterObjectName(Name);
        }

        Name = InName;

        if (OwningWorld && !Name.empty())
        {
            OwningWorld->RegisterObjectName(Name, shared_from_this());
        }
    }

    void WorldObject::Tick(float DeltaTimeS)
    {
        ComponentsMgr.Tick(DeltaTimeS);
    }

    void WorldObject::Render()
    {
        ComponentsMgr.Render();
    }

    TransformComponent* WorldObject::Transform()
    {
        return ComponentsMgr.Get<TransformComponent>().get();
    }

    const EngineContext& WorldObject::GetContext() const
    {
        return *Context;
    }
}
