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
            OwningWorld->Objects().UnregisterName(Name);
        }

        Name = InName;

        if (OwningWorld && !Name.empty())
        {
            OwningWorld->Objects().RegisterName(Name, shared_from_this());
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

    WorldCoordinate WorldObject::WorldToLocal(const WorldCoordinate& WorldPos) const
    {
        sf::Vector2f ObjectPosition(0.0f, 0.0f);
        if (std::shared_ptr<TransformComponent> TransformComp = ComponentsMgr.Get<TransformComponent>())
        {
            ObjectPosition = TransformComp->Position;
        }
        return WorldCoordinate(WorldPos.Value.x - ObjectPosition.x, WorldPos.Value.y - ObjectPosition.y);
    }

    WorldCoordinate WorldObject::LocalToWorld(const WorldCoordinate& LocalPos) const
    {
        sf::Vector2f ObjectPosition(0.0f, 0.0f);
        if (std::shared_ptr<TransformComponent> TransformComp = ComponentsMgr.Get<TransformComponent>())
        {
            ObjectPosition = TransformComp->Position;
        }
        return WorldCoordinate(LocalPos.Value.x + ObjectPosition.x, LocalPos.Value.y + ObjectPosition.y);
    }

    nlohmann::json WorldObject::ToJson() const
    {
        nlohmann::json ObjectJson;

        if (!Name.empty())
        {
            ObjectJson["name"] = Name;
        }

        nlohmann::json ComponentsArray = nlohmann::json::array();

        for (const auto& [TypeIndex, ComponentPtr] : ComponentsMgr.GetAll())
        {
            if (!ComponentPtr)
                continue;

            nlohmann::json ComponentJson;
            ComponentJson["type"] = ComponentPtr->GetName();
            ComponentJson["data"] = ComponentPtr->ToJson();

            ComponentsArray.push_back(ComponentJson);
        }

        ObjectJson["components"] = ComponentsArray;

        return ObjectJson;
    }
}
