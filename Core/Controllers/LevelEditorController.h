#pragma once

#include "Controller.h"
#include "../Coordinates/WindowCoordinate.h"
#include "../Coordinates/WorldCoordinate.h"

namespace Core
{
    class CameraComponent;
    class TileMapComponent;
}

namespace Game
{
    class LevelDesignerScene;

    class LevelEditorController : public Core::Controller
    {
    public:
        LevelEditorController(const std::shared_ptr<Core::WorldObject>& Owner, std::shared_ptr<Core::EngineContext> Context);

        void Start() override;

        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& Event) override;
        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& Event) override;
        void OnMouseMoved(const sf::Event::MouseMoved& Event) override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& Event) override;

    private:
        std::weak_ptr<Core::CameraComponent> Camera;
        std::weak_ptr<LevelDesignerScene> Scene;
        std::weak_ptr<Core::TileMapComponent> TileMap;

        bool bIsPanning = false;
        Core::WorldCoordinate LastPanWorldPos;
    };
}
