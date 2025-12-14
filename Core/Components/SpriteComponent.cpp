#include "SpriteComponent.h"

#include "ComponentRegistry.h"
#include "TransformComponent.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/AssetRegistrySystem.h"

namespace Core
{
    REGISTER_COMPONENT(SpriteComponent);

    SpriteComponent::SpriteComponent(const std::shared_ptr<WorldObject>& Owner)
        : Component(Owner)
    {
    }

    bool SpriteComponent::Initialize(const nlohmann::json& Data)
    {
        const EngineContext& Context = GetContext();
        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context.SystemsRegistry->GetCoreSystem<
            AssetRegistrySystem>();

        const std::string TextureAssetId{Data.value("texture", "")};
        if (TextureAssetId.empty())
        {
            return false;
        }

        std::shared_ptr<const sf::Texture> Texture = AssetRegistry->Get<sf::Texture>(TextureAssetId);
        if (!Texture)
        {
            return false;
        }

        Sprite = std::make_shared<sf::Sprite>(*Texture);
        Sprite->setScale({1.f, 1.f});
        return true;
    }

    void SpriteComponent::Render()
    {
        if (TransformComponent* Transform = GetComponent<TransformComponent>())
        {
            Sprite->setPosition(Transform->Position);
        }
        GetContext().Window->draw(*Sprite);
    }
}
