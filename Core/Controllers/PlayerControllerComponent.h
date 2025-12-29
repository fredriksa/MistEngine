#pragma once

#include "Controller.h"
#include <memory>

namespace Core
{
    class WorldObject;

    class PlayerControllerComponent : public Controller
    {
    public:
        PlayerControllerComponent(const std::shared_ptr<Core::WorldObject>& Owner, std::shared_ptr<Core::EngineContext> Context);

        void Start() override;
        void Tick(float DeltaTimeS) override;

        void OnKeyPressed(const sf::Event::KeyPressed& Event) override;
        void OnKeyReleased(const sf::Event::KeyReleased& Event) override;

        void Possess(const std::shared_ptr<WorldObject>& Pawn);
        void Unpossess();
        std::shared_ptr<WorldObject> GetPossessedPawn() const;

        void SetMovementSpeed(float Speed) { MovementSpeed = Speed; }
        float GetMovementSpeed() const { return MovementSpeed; }

    private:
        std::weak_ptr<WorldObject> PossessedPawn;
        float MovementSpeed = 200.0f;

        bool bMovingLeft = false;
        bool bMovingRight = false;
        bool bMovingUp = false;
        bool bMovingDown = false;
    };
}
