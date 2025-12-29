#include "PlayerCharacterComponent.h"
#include "ComponentRegistry.h"
#include "TransformComponent.h"
#include "../World/WorldObject.h"
#include <cmath>

namespace Core
{
    REGISTER_COMPONENT(PlayerCharacterComponent);

    PlayerCharacterComponent::PlayerCharacterComponent(const std::shared_ptr<WorldObject>& Owner,
                                                       std::shared_ptr<EngineContext> Context)
        : Component(Owner, std::move(Context), "PlayerCharacterComponent")
    {
    }

    void PlayerCharacterComponent::Start()
    {
    }

    void PlayerCharacterComponent::Tick(float DeltaTimeS)
    {
        if (AccumulatedMovementInput.x == 0.0f && AccumulatedMovementInput.y == 0.0f)
            return;

        sf::Vector2f Movement = AccumulatedMovementInput;

        const float Length = std::sqrt(Movement.x * Movement.x + Movement.y * Movement.y);
        if (Length > 0.0f)
        {
            Movement.x /= Length;
            Movement.y /= Length;
        }

        Movement.x *= MovementSpeed * DeltaTimeS;
        Movement.y *= MovementSpeed * DeltaTimeS;

        if (std::shared_ptr<TransformComponent> Transform = GetOwner()->Components().Get<TransformComponent>())
        {
            Transform->Position += Movement;
        }

        AccumulatedMovementInput = sf::Vector2f(0.0f, 0.0f);
    }

    void PlayerCharacterComponent::AddMovementInput(sf::Vector2f Direction, float Scale)
    {
        AccumulatedMovementInput += Direction * Scale;
    }
}
