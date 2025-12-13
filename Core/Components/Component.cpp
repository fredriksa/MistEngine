#include "Component.h"

#include "../Systems/AssetRegistrySystem.h"

namespace Core
{
    Component::Component(const std::shared_ptr<WorldObject>& Owner)
        : OwnerWeak(Owner)
    {
    }

    WorldObject* Component::GetOwner() const
    {
        if (std::shared_ptr<WorldObject> Locked = OwnerWeak.lock())
        {
            return Locked.get();
        }
        return nullptr;
    }
}
