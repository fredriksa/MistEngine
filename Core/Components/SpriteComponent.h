#pragma once

#include "Component.h"
#include "ComponentRegistry.h"
#include "SFML/Graphics.hpp"

namespace Core
{
    class SpriteComponent : public Component
    {
    public:
        SpriteComponent(const std::shared_ptr<WorldObject>& Owner);

        bool Initialize(const nlohmann::json& Data) override;
        void Render() override;

    private:
        std::shared_ptr<sf::Sprite> Sprite = nullptr;
    };
}
