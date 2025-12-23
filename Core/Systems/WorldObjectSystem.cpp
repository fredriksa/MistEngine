#include "WorldObjectSystem.h"

#include "../Assets/DataAsset.h"
#include "../Components/ComponentRegistry.h"
#include "../Components/TransformComponent.h"
#include "../World/WorldObject.h"
#include "../World/World.h"
#include "../Utils/JsonUtils.h"

namespace Core
{
    WorldObjectSystem::WorldObjectSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("WorldObjectSystem", Type, std::move(InContext))
    {
    }

    std::shared_ptr<WorldObject> WorldObjectSystem::Create(World* TargetWorld, const DataAsset& DataAsset,
                                                           const nlohmann::json& OverrideValues)
    {
        std::shared_ptr<WorldObject> WorldObj = TargetWorld->CreateObject();

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
                AssetComponentData.Type, WorldObj, WorldObj->GetContextPtr());
            Component->Initialize(MergedAssetComponentData);
            WorldObj->Components().Attach(Component);
        }

        if (OverrideValues.contains("components") && OverrideValues["components"].is_array())
        {
            for (const auto& OverrideComp : OverrideValues["components"])
            {
                std::string ComponentType = OverrideComp["type"].get<std::string>();
                bool ExistsInTemplate = false;

                for (const ComponentData& AssetComponentData : DataAsset.Components)
                {
                    if (AssetComponentData.Type == ComponentType)
                    {
                        ExistsInTemplate = true;
                        break;
                    }
                }

                if (!ExistsInTemplate)
                {
                    std::shared_ptr<Component> Component = ComponentRegistry::Get().Create(
                        ComponentType, WorldObj, WorldObj->GetContextPtr());
                    Component->Initialize(OverrideComp["data"]);
                    WorldObj->Components().Attach(Component);
                }
            }
        }

        if (!WorldObj->Components().Get<TransformComponent>())
        {
            WorldObj->Components().Add<TransformComponent>();
        }

        return WorldObj;
    }

    std::shared_ptr<WorldObject> WorldObjectSystem::Create(World* TargetWorld, const nlohmann::json& ObjectData)
    {
        std::shared_ptr<WorldObject> WorldObj = TargetWorld->CreateObject();

        if (ObjectData.contains("components") && ObjectData["components"].is_array())
        {
            for (const auto& ComponentJson : ObjectData["components"])
            {
                std::string ComponentType = ComponentJson["type"].get<std::string>();
                std::shared_ptr<Component> Component = ComponentRegistry::Get().Create(ComponentType, WorldObj, WorldObj->GetContextPtr());
                Component->Initialize(ComponentJson["data"]);
                WorldObj->Components().Attach(Component);
            }
        }

        if (!WorldObj->Components().Get<TransformComponent>())
        {
            WorldObj->Components().Add<TransformComponent>();
        }

        return WorldObj;
    }
}
