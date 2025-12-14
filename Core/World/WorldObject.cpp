#include "WorldObject.h"

#include "../Components/TransformComponent.h"

namespace Core
{
    WorldObject::WorldObject(std::shared_ptr<EngineContext> Context)
        : Context(std::move(Context))
    {
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
