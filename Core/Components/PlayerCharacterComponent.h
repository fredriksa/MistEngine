#pragma once

#include "Component.h"
#include <SFML/System/Vector2.hpp>

namespace Core
{
    class PlayerCharacterComponent : public Component
    {
    public:
        PlayerCharacterComponent(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context);

        void Start() override;
        void Tick(float DeltaTimeS) override;

        void AddMovementInput(sf::Vector2f Direction, float Scale = 1.0f);

        void SetMovementSpeed(float Speed) { MovementSpeed = Speed; }
        float GetMovementSpeed() const { return MovementSpeed; }

    private:
        sf::Vector2f AccumulatedMovementInput{0.0f, 0.0f};
        float MovementSpeed = 200.0f;
    };
}
