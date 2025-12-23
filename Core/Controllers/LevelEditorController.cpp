#include "LevelEditorController.h"
#include "../../Game/Scenes/LevelDesigner.h"
#include "../Components/CameraComponent.h"
#include "../Components/ComponentRegistry.h"
#include "../Components/TileMapComponent.h"
#include "../Components/TransformComponent.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/CoordinateProjectionSystem.h"
#include "../Systems/SceneManagerSystem.h"
#include "../World/WorldObject.h"
#include "../World/World.h"
#include "../World/ComponentManager.h"
#include "../TileMap/Tile.h"
#include "imgui.h"
#include <algorithm>

namespace Game
{
    REGISTER_COMPONENT(LevelEditorController);

    LevelEditorController::LevelEditorController(const std::shared_ptr<Core::WorldObject>& Owner, std::shared_ptr<Core::EngineContext> Context)
        : Controller(Owner, std::move(Context), Core::InputSource::KeyboardMouse, "LevelEditorController")
    {
    }

    void LevelEditorController::Start()
    {
        Camera = GetOwner()->Components().Get<Core::CameraComponent>();

        if (std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock())
        {
            CamPtr->SetZoom(InitialZoom);
        }

        if (GetOwner()->Transform())
        {
            InitialCameraPosition = GetOwner()->Transform()->Position;
        }

        if (std::shared_ptr<Core::WorldObject> TileMapObject = GetOwner()->GetWorld()->GetObjectByName("TileMap"))
        {
            TileMap = TileMapObject->Components().Get<Core::TileMapComponent>();
        }

        if (std::shared_ptr<Core::SceneManagerSystem> SceneManager = GetContext().SystemsRegistry->GetCoreSystem<
            Core::SceneManagerSystem>())
        {
            Scene = std::dynamic_pointer_cast<LevelDesignerScene>(SceneManager->GetActiveScene());
        }
    }

    void LevelEditorController::OnMouseButtonPressed(const sf::Event::MouseButtonPressed& Event)
    {
        if (ImGui::IsAnyItemHovered())
        {
            return;
        }

        Core::WindowCoordinate WindowCoords(Event.position.x, Event.position.y);

        if (Event.button == sf::Mouse::Button::Left)
        {
            bIsPainting = true;
            PaintTileAtMousePosition(WindowCoords);
        }
        else if (Event.button == sf::Mouse::Button::Middle)
        {
            std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock();
            std::shared_ptr<Core::CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
                Core::CoordinateProjectionSystem>();

            if (!CamPtr || !Projector)
                return;

            bIsPanning = true;
            LastPanWorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());
        }
    }

    void LevelEditorController::OnMouseButtonReleased(const sf::Event::MouseButtonReleased& Event)
    {
        if (Event.button == sf::Mouse::Button::Left)
        {
            bIsPainting = false;
        }
        else if (Event.button == sf::Mouse::Button::Middle)
        {
            bIsPanning = false;
        }
    }

    void LevelEditorController::OnMouseMoved(const sf::Event::MouseMoved& Event)
    {
        Core::WindowCoordinate WindowCoords(Event.position.x, Event.position.y);
        LastMousePosition = WindowCoords;

        if (bIsPainting)
        {
            PaintTileAtMousePosition(WindowCoords);
        }

        if (bIsPanning)
        {
            std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock();
            std::shared_ptr<Core::CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
                Core::CoordinateProjectionSystem>();

            if (!CamPtr || !Projector)
                return;

            Core::WorldCoordinate CurrentWorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());

            Core::WorldCoordinate Delta = LastPanWorldPos - CurrentWorldPos;

            GetOwner()->Transform()->Position += Delta.Value;

            LastPanWorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());
        }
    }

    void LevelEditorController::OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& Event)
    {
        std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<Core::CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        Core::WindowCoordinate MousePos(Event.position.x, Event.position.y);

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;
        Core::WorldCoordinate WorldPosBeforeZoom = Projector->WindowToWorld(MousePos, CamPtr->GetView());

        float CurrentZoom = CamPtr->GetZoom();
        float NewZoom = CurrentZoom * (1.0f - Event.delta * 0.1f);
        NewZoom = std::clamp(NewZoom, 0.25f, 4.0f);

        CamPtr->SetZoom(NewZoom);

        Core::WorldCoordinate WorldPosAfterZoom = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        Core::WorldCoordinate Delta = WorldPosBeforeZoom - WorldPosAfterZoom;

        GetOwner()->Transform()->Position += Delta.Value;
    }

    void LevelEditorController::OnKeyPressed(const sf::Event::KeyPressed& Event)
    {
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock();

        if (!ScenePtr)
            return;

        if (Event.code == sf::Keyboard::Key::G)
        {
            ScenePtr->ToggleGrid();
        }
        else if (Event.code == sf::Keyboard::Key::Equal)
        {
            if (CamPtr)
            {
                float CurrentZoom = CamPtr->GetZoom();
                float NewZoom = CurrentZoom * 0.9f;
                NewZoom = std::clamp(NewZoom, 0.25f, 4.0f);
                CamPtr->SetZoom(NewZoom);
            }
        }
        else if (Event.code == sf::Keyboard::Key::Hyphen)
        {
            if (CamPtr)
            {
                float CurrentZoom = CamPtr->GetZoom();
                float NewZoom = CurrentZoom * 1.1f;
                NewZoom = std::clamp(NewZoom, 0.25f, 4.0f);
                CamPtr->SetZoom(NewZoom);
            }
        }
        else if (Event.code == sf::Keyboard::Key::R)
        {
            if (CamPtr)
            {
                CamPtr->SetZoom(InitialZoom);
            }
            if (GetOwner()->Transform())
            {
                GetOwner()->Transform()->Position = InitialCameraPosition;
            }
        }
        else if (Event.code == sf::Keyboard::Key::D)
        {
            DeleteTileAtMousePosition(LastMousePosition);
        }
    }

    void LevelEditorController::PaintTileAtMousePosition(Core::WindowCoordinate MousePos)
    {
        std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<Core::CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        std::shared_ptr<Core::TileMapComponent> TileMapPtr = ScenePtr->GetSelectedTileMap();
        if (!TileMapPtr)
            return;

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;

        Core::WorldCoordinate WorldCoords = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        Core::WorldCoordinate LocalCoords = TileMapPtr->GetOwner()->WorldToLocal(WorldCoords);
        Core::TileCoordinate TileCoords = Projector->WorldToTile(LocalCoords);

        if (!TileCoords.IsValid())
            return;

        Game::TileSelection Selection = ScenePtr->GetCurrentSelection();
        if (!Selection.IsValid())
            return;

        int TileSheetColumns = ScenePtr->GetTileSheetColumns(Selection.TileSheetIndex.value());
        if (TileSheetColumns == 0)
            return;

        Core::uint CurrentLayer = ScenePtr->GetCurrentLayer();

        for (int OffsetY = 0; OffsetY < Selection.SelectionRect.Height(); ++OffsetY)
        {
            for (int OffsetX = 0; OffsetX < Selection.SelectionRect.Width(); ++OffsetX)
            {
                Core::uint TargetX = TileCoords.X() + OffsetX;
                Core::uint TargetY = TileCoords.Y() + OffsetY;

                if (TargetX >= TileMapPtr->GetTileMap().GetWidth() ||
                    TargetY >= TileMapPtr->GetTileMap().GetHeight())
                    continue;

                int TileIndex = Selection.GetTileIndex(sf::Vector2i(OffsetX, OffsetY), TileSheetColumns);
                TileMapPtr->GetTileMap().SetTile(TargetX, TargetY, CurrentLayer,
                    Core::Tile(Selection.TileSheetIndex.value(), TileIndex));
            }
        }
    }

    void LevelEditorController::DeleteTileAtMousePosition(Core::WindowCoordinate MousePos)
    {
        std::shared_ptr<Core::CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<Core::CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        std::shared_ptr<Core::TileMapComponent> TileMapPtr = ScenePtr->GetSelectedTileMap();
        if (!TileMapPtr)
            return;

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;

        Core::WorldCoordinate WorldCoords = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        Core::WorldCoordinate LocalCoords = TileMapPtr->GetOwner()->WorldToLocal(WorldCoords);
        Core::TileCoordinate TileCoords = Projector->WorldToTile(LocalCoords);

        if (!TileCoords.IsValid())
            return;

        Core::uint CurrentLayer = ScenePtr->GetCurrentLayer();
        TileMapPtr->GetTileMap().SetTile(TileCoords.X(), TileCoords.Y(), CurrentLayer, Core::Tile());
    }
}
