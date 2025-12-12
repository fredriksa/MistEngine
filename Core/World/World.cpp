#include "World.h"

namespace Core
{
    World::World(std::shared_ptr<Core::EngineContext> Context)
        : Context(std::move(Context))
    {
    }

    void World::Tick(float DeltaTimeS)
    {
        for (const std::shared_ptr<WorldObject>& WorldObject : WorldObjects)
        {
            WorldObject->Tick(DeltaTimeS);
        }
    }

    void World::Render()
    {
        for (const std::shared_ptr<WorldObject>& WorldObject : WorldObjects)
        {
            WorldObject->Render();
        }
    }
}
