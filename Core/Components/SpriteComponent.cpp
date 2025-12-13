#include "SpriteComponent.h"

#include "ComponentRegistry.h"
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
        const auto Size = static_cast<sf::Vector2f>(GetContext().WindowSize);
        const auto HalfSize = sf::Vector2f(Size.x / 2, Size.y / 2);
        Sprite->setPosition(HalfSize);
        return true;
    }

    void SpriteComponent::Render()
    {
        GetContext().Window->draw(*Sprite);
    }
}
