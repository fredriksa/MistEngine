#include "WorldObjectSystem.h"

#include "../Assets/DataAsset.h"
#include "../Components/ComponentRegistry.h"
#include "../World/WorldObject.h"
#include "../Utils/JsonUtils.h"

namespace Core
{
    WorldObjectSystem::WorldObjectSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("WorldObjectSystem", Type, std::move(InContext))
    {
    }

    std::shared_ptr<WorldObject> WorldObjectSystem::Create(const DataAsset& DataAsset,
                                                           const nlohmann::json& OverrideValues)
    {
        std::shared_ptr<WorldObject> WorldObj = std::make_shared<WorldObject>(GetContext());
        for (const ComponentData& AssetComponentData : DataAsset.Components)
        {
            nlohmann::json MergedAssetComponentData = AssetComponentData.Data;
            if (OverrideValues.contains("components") && OverrideValues["components"].is_array())
            {
                for (const auto& OverrideComp : OverrideValues["components"])
                {
                    if (OverrideComp["type"] == AssetComponentData.Type)
                    {
                        MergedAssetComponentData = Merge(AssetComponentData.Data, OverrideComp["data"]);
                        break;
                    }
                }
            }

            std::shared_ptr<Component> Component = ComponentRegistry::Get().Create(
                AssetComponentData.Type, WorldObj);
            Component->Initialize(MergedAssetComponentData);
            WorldObj->Components().Attach(Component);
        }
        return WorldObj;
    }
}
