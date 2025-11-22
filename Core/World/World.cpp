#include "World.h"

namespace Core
{
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
