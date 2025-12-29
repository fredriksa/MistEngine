#include "PlayerControllerComponent.h"
#include "../World/WorldObject.h"
#include "../Components/PlayerCharacterComponent.h"
#include "../Components/ComponentRegistry.h"

namespace Core
{
    REGISTER_COMPONENT(PlayerControllerComponent);

    PlayerControllerComponent::PlayerControllerComponent(const std::shared_ptr<Core::WorldObject>& Owner, std::shared_ptr<Core::EngineContext> Context)
        : Controller(Owner, std::move(Context), InputSource::KeyboardMouse, "PlayerControllerComponent")
    {
    }

    void PlayerControllerComponent::Start()
    {
    }

    void PlayerControllerComponent::Tick(float DeltaTimeS)
    {
        std::shared_ptr<WorldObject> Pawn = PossessedPawn.lock();
        if (!Pawn)
            return;

        std::shared_ptr<PlayerCharacterComponent> Character = Pawn->Components().Get<PlayerCharacterComponent>();
        if (!Character)
            return;

        sf::Vector2f MovementDirection(0.0f, 0.0f);

        if (bMovingLeft)
            MovementDirection.x -= 1.0f;
        if (bMovingRight)
            MovementDirection.x += 1.0f;
        if (bMovingUp)
            MovementDirection.y -= 1.0f;
        if (bMovingDown)
            MovementDirection.y += 1.0f;

        if (MovementDirection.x != 0.0f || MovementDirection.y != 0.0f)
        {
            Character->AddMovementInput(MovementDirection);
        }
    }

    void PlayerControllerComponent::OnKeyPressed(const sf::Event::KeyPressed& Event)
    {
        if (Event.code == sf::Keyboard::Key::W || Event.code == sf::Keyboard::Key::Up)
        {
            bMovingUp = true;
        }
        else if (Event.code == sf::Keyboard::Key::S || Event.code == sf::Keyboard::Key::Down)
        {
            bMovingDown = true;
        }
        else if (Event.code == sf::Keyboard::Key::A || Event.code == sf::Keyboard::Key::Left)
        {
            bMovingLeft = true;
        }
        else if (Event.code == sf::Keyboard::Key::D || Event.code == sf::Keyboard::Key::Right)
        {
            bMovingRight = true;
        }
    }

    void PlayerControllerComponent::OnKeyReleased(const sf::Event::KeyReleased& Event)
    {
        if (Event.code == sf::Keyboard::Key::W || Event.code == sf::Keyboard::Key::Up)
        {
            bMovingUp = false;
        }
        else if (Event.code == sf::Keyboard::Key::S || Event.code == sf::Keyboard::Key::Down)
        {
            bMovingDown = false;
        }
        else if (Event.code == sf::Keyboard::Key::A || Event.code == sf::Keyboard::Key::Left)
        {
            bMovingLeft = false;
        }
        else if (Event.code == sf::Keyboard::Key::D || Event.code == sf::Keyboard::Key::Right)
        {
            bMovingRight = false;
        }
    }

    void PlayerControllerComponent::Possess(const std::shared_ptr<WorldObject>& Pawn)
    {
        PossessedPawn = Pawn;
    }

    void PlayerControllerComponent::Unpossess()
    {
        PossessedPawn.reset();
    }

    std::shared_ptr<WorldObject> PlayerControllerComponent::GetPossessedPawn() const
    {
        return PossessedPawn.lock();
    }
}
