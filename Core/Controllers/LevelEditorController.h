#pragma once

#include "Controller.h"
#include "../Coordinates/WindowCoordinate.h"
#include "../Coordinates/WorldCoordinate.h"

namespace Core
{
    class CameraComponent;
    class TileMapComponent;
    class LevelDesignerScene;

    class LevelEditorController : public Controller
    {
    public:
        LevelEditorController(const std::shared_ptr<Core::WorldObject>& Owner, std::shared_ptr<Core::EngineContext> Context);

        void Start() override;

        void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& Event) override;
        void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& Event) override;
        void OnMouseMoved(const sf::Event::MouseMoved& Event) override;
        void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& Event) override;
        void OnKeyPressed(const sf::Event::KeyPressed& Event) override;

    private:
        void PaintTileAtMousePosition(WindowCoordinate MousePos);
        void DeleteTileAtMousePosition(WindowCoordinate MousePos);

        std::weak_ptr<CameraComponent> Camera;
        std::weak_ptr<LevelDesignerScene> Scene;
        std::weak_ptr<TileMapComponent> TileMap;

        bool bIsPanning = false;
        bool bIsPainting = false;
        WindowCoordinate LastMousePosition{0, 0};
        WorldCoordinate LastPanWorldPos;

        bool bSelectRectCtrlHeld = false;

        sf::Vector2f InitialCameraPosition{0.0f, 0.0f};
        float InitialZoom = 0.25f;
    };
}
