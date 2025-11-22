#include "WorldObject.h"

#include "../Components/Component.h"

#include <ranges>

namespace Core
{
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
}
