#include "AssetRegistrySystem.h"

namespace Core
{
    AssetRegistrySystem::AssetRegistrySystem(const std::shared_ptr<EngineContext>& InContext)
        : CoreSystem("AssetRegistrySystem", Type, InContext)
    {
    }
}
