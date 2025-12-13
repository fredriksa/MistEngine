#include "WorldObject.h"

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

    const EngineContext& WorldObject::GetContext() const
    {
        return *Context;
    }
}
