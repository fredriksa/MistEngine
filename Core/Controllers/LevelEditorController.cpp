#include "LevelEditorController.h"
#include "../Editor/LevelDesignerScene.h"
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
#include "../TileMap/TileMap.h"
#include "imgui.h"
#include <algorithm>

namespace Core
{
    REGISTER_COMPONENT(LevelEditorController);

    LevelEditorController::LevelEditorController(const std::shared_ptr<Core::WorldObject>& Owner, std::shared_ptr<Core::EngineContext> Context)
        : Controller(Owner, std::move(Context), InputSource::KeyboardMouse, "LevelEditorController")
    {
    }

    void LevelEditorController::Start()
    {
        Camera = GetOwner()->Components().Get<Core::CameraComponent>();

        if (std::shared_ptr<CameraComponent> CamPtr = Camera.lock())
        {
            CamPtr->SetZoom(InitialZoom);
        }

        if (GetOwner()->Transform())
        {
            InitialCameraPosition = GetOwner()->Transform()->Position;
        }

        if (std::shared_ptr<Core::WorldObject> TileMapObject = GetOwner()->GetWorld()->Objects().GetByName("TileMap"))
        {
            TileMap = TileMapObject->Components().Get<TileMapComponent>();
        }

        if (std::shared_ptr<SceneManagerSystem> SceneManager = GetContext().SystemsRegistry->GetCoreSystem<
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

        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        if (!ScenePtr)
            return;

        WindowCoordinate WindowCoords(Event.position.x, Event.position.y);

        if (Event.button == sf::Mouse::Button::Left)
        {
            EditorTool CurrentTool = ScenePtr->GetModel().GetCurrentTool();

            if (CurrentTool == EditorTool::Brush)
            {
                bIsPainting = true;
                PaintTile(WindowCoords);
            }
            else if (CurrentTool == EditorTool::Eraser)
            {
                bIsPainting = true;
                DeleteTile(WindowCoords);
            }
            else if (CurrentTool == EditorTool::Eyedropper)
            {
                EyedropperTile(WindowCoords);
            }
            else if (CurrentTool == EditorTool::Fill)
            {
                FloodFill(WindowCoords);
            }
            else if (CurrentTool == EditorTool::Select)
            {
                std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
                std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
                    Core::CoordinateProjectionSystem>();

                if (!CamPtr || !Projector)
                    return;

                if (!ScenePtr->IsClickInCanvas(WindowCoords))
                    return;

                const bool bCtrlHeld = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) ||
                                       sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);

                WorldCoordinate WorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());

                GizmoPart ClickedGizmoPart = ScenePtr->GetModel().GetGizmoPartAtPosition(WorldPos);
                if (ClickedGizmoPart != GizmoPart::None)
                {
                    DragMode Mode = DragMode::XY;
                    if (ClickedGizmoPart == GizmoPart::XAxis)
                        Mode = DragMode::XOnly;
                    else if (ClickedGizmoPart == GizmoPart::YAxis)
                        Mode = DragMode::YOnly;

                    ScenePtr->GetModel().StartDraggingObjects(WorldPos, Mode);
                }
                else
                {
                    WorldObject* HitObject = ScenePtr->GetModel().GetObjectAtPosition(WorldPos);
                    if (HitObject)
                    {
                        if (bCtrlHeld)
                        {
                            ScenePtr->GetModel().GetSelection().Toggle(HitObject->shared_from_this());
                        }
                        else
                        {
                            if (!ScenePtr->GetModel().GetSelection().Contains(HitObject))
                            {
                                ScenePtr->GetModel().GetSelection().Clear();
                                ScenePtr->GetModel().GetSelection().Add(HitObject->shared_from_this());
                            }
                        }
                    }
                    else
                    {
                        if (!bCtrlHeld)
                        {
                            ScenePtr->GetModel().GetSelection().Clear();
                        }
                        ScenePtr->GetModel().StartSelectionRectangle(WindowCoords);
                        bSelectRectCtrlHeld = bCtrlHeld;
                    }
                }
            }
        }
        else if (Event.button == sf::Mouse::Button::Middle)
        {
            std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
            std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
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

            std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
            if (ScenePtr)
            {
                if (ScenePtr->GetModel().IsDraggingObjects())
                {
                    ScenePtr->GetModel().EndDraggingObjects();
                }
                else if (ScenePtr->GetModel().IsSelectingRectangle())
                {
                    std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
                    std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
                        Core::CoordinateProjectionSystem>();

                    if (CamPtr && Projector)
                    {
                        WindowCoordinate SelectRectStart = ScenePtr->GetModel().GetSelectionRectStart();
                        WindowCoordinate SelectRectEnd = ScenePtr->GetModel().GetSelectionRectCurrent();

                        WorldCoordinate WorldStart = Projector->WindowToWorld(SelectRectStart, CamPtr->GetView());
                        WorldCoordinate WorldEnd = Projector->WindowToWorld(SelectRectEnd, CamPtr->GetView());

                        const float MinX = std::min(WorldStart.X(), WorldEnd.X());
                        const float MaxX = std::max(WorldStart.X(), WorldEnd.X());
                        const float MinY = std::min(WorldStart.Y(), WorldEnd.Y());
                        const float MaxY = std::max(WorldStart.Y(), WorldEnd.Y());

                        WorldCoordinate TopLeft(MinX, MinY);
                        WorldCoordinate BottomRight(MaxX, MaxY);

                        ScenePtr->GetModel().SelectObjectsInRectangle(TopLeft, BottomRight, bSelectRectCtrlHeld);
                    }

                    ScenePtr->GetModel().ClearSelectionRectangle();
                }
            }
        }
        else if (Event.button == sf::Mouse::Button::Middle)
        {
            bIsPanning = false;
        }
    }

    void LevelEditorController::OnMouseMoved(const sf::Event::MouseMoved& Event)
    {
        WindowCoordinate WindowCoords(Event.position.x, Event.position.y);
        LastMousePosition = WindowCoords;

        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();

        if (bIsPainting && ScenePtr)
        {
            EditorTool CurrentTool = ScenePtr->GetModel().GetCurrentTool();
            if (CurrentTool == EditorTool::Brush)
            {
                PaintTile(WindowCoords);
            }
            else if (CurrentTool == EditorTool::Eraser)
            {
                DeleteTile(WindowCoords);
            }
        }

        if (bIsPanning)
        {
            std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
            std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
                Core::CoordinateProjectionSystem>();

            if (!CamPtr || !Projector)
                return;

            WorldCoordinate CurrentWorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());

            WorldCoordinate Delta = LastPanWorldPos - CurrentWorldPos;

            GetOwner()->Transform()->Position += Delta.Value;

            LastPanWorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());
        }

        if (ScenePtr && ScenePtr->GetModel().GetCurrentTool() == EditorTool::Select)
        {
            std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
            std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
                Core::CoordinateProjectionSystem>();

            if (!CamPtr || !Projector)
                return;

            WorldCoordinate WorldPos = Projector->WindowToWorld(WindowCoords, CamPtr->GetView());

            if (ScenePtr->GetModel().IsDraggingObjects())
            {
                ScenePtr->GetModel().UpdateDraggedObjects(WorldPos);
            }
            else if (ScenePtr->GetModel().IsSelectingRectangle())
            {
                ScenePtr->GetModel().UpdateSelectionRectangle(WindowCoords);
            }
            else if (ScenePtr->IsClickInCanvas(WindowCoords))
            {
                ScenePtr->GetModel().SetHoveredObject(WorldPos);
            }
            else
            {
                ScenePtr->GetModel().ClearHoveredObject();
            }
        }
    }

    void LevelEditorController::OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& Event)
    {
        std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        WindowCoordinate MousePos(Event.position.x, Event.position.y);

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;
        WorldCoordinate WorldPosBeforeZoom = Projector->WindowToWorld(MousePos, CamPtr->GetView());

        float CurrentZoom = CamPtr->GetZoom();
        float NewZoom = CurrentZoom * (1.0f - Event.delta * 0.1f);
        NewZoom = std::clamp(NewZoom, 0.25f, 4.0f);

        CamPtr->SetZoom(NewZoom);

        WorldCoordinate WorldPosAfterZoom = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        WorldCoordinate Delta = WorldPosBeforeZoom - WorldPosAfterZoom;

        GetOwner()->Transform()->Position += Delta.Value;
    }

    void LevelEditorController::OnKeyPressed(const sf::Event::KeyPressed& Event)
    {
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<CameraComponent> CamPtr = Camera.lock();

        if (!ScenePtr)
            return;

        if (Event.code == sf::Keyboard::Key::G)
        {
            ScenePtr->GetModel().ToggleGrid();
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
        else if (Event.code == sf::Keyboard::Key::S)
        {
            ScenePtr->GetModel().SetCurrentTool(EditorTool::Select);
        }

        std::shared_ptr<TileMapComponent> TileMapPtr = ScenePtr->GetModel().GetSelectedTileMap();
        if (TileMapPtr)
        {
            if (Event.code == sf::Keyboard::Key::B)
            {
                ScenePtr->GetModel().SetCurrentTool(EditorTool::Brush);
            }
            else if (Event.code == sf::Keyboard::Key::F)
            {
                ScenePtr->GetModel().SetCurrentTool(EditorTool::Fill);
            }
            else if (Event.code == sf::Keyboard::Key::D)
            {
                ScenePtr->GetModel().SetCurrentTool(EditorTool::Eraser);
            }
            else if (Event.code == sf::Keyboard::Key::E)
            {
                ScenePtr->GetModel().SetCurrentTool(EditorTool::Eyedropper);
            }
        }
    }

    void LevelEditorController::PaintTile(WindowCoordinate MousePos)
    {
        std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        std::shared_ptr<TileMapComponent> TileMapPtr = ScenePtr->GetModel().GetSelectedTileMap();
        if (!TileMapPtr)
            return;

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;

        WorldCoordinate WorldCoords = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        WorldCoordinate LocalCoords = TileMapPtr->GetOwner()->WorldToLocal(WorldCoords);
        TileCoordinate TileCoords = Projector->WorldToTile(LocalCoords);

        if (!TileCoords.IsValid())
            return;

        TileSelection Selection = ScenePtr->GetModel().GetCurrentSelection();
        if (!Selection.IsValid())
            return;

        int TileSheetColumns = ScenePtr->GetModel().GetTileSheetColumns(Selection.TileSheetIndex.value());
        if (TileSheetColumns == 0)
            return;

        Core::uint CurrentLayer = ScenePtr->GetModel().GetCurrentLayer();

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
                    Tile(Selection.TileSheetIndex.value(), TileIndex));
            }
        }
    }

    void LevelEditorController::DeleteTile(WindowCoordinate MousePos)
    {
        std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        std::shared_ptr<TileMapComponent> TileMapPtr = ScenePtr->GetModel().GetSelectedTileMap();
        if (!TileMapPtr)
            return;

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;

        WorldCoordinate WorldCoords = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        WorldCoordinate LocalCoords = TileMapPtr->GetOwner()->WorldToLocal(WorldCoords);
        TileCoordinate TileCoords = Projector->WorldToTile(LocalCoords);

        if (!TileCoords.IsValid())
            return;

        Core::uint CurrentLayer = ScenePtr->GetModel().GetCurrentLayer();
        TileMapPtr->GetTileMap().SetTile(TileCoords.X(), TileCoords.Y(), CurrentLayer, Tile());
    }

    void LevelEditorController::EyedropperTile(WindowCoordinate MousePos)
    {
        std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        std::shared_ptr<TileMapComponent> TileMapPtr = ScenePtr->GetModel().GetSelectedTileMap();
        if (!TileMapPtr)
            return;

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;

        WorldCoordinate WorldCoords = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        WorldCoordinate LocalCoords = TileMapPtr->GetOwner()->WorldToLocal(WorldCoords);
        TileCoordinate TileCoords = Projector->WorldToTile(LocalCoords);

        if (!TileCoords.IsValid())
            return;

        Core::uint CurrentLayer = ScenePtr->GetModel().GetCurrentLayer();
        Tile SampledTile = TileMapPtr->GetTileMap().GetTile(TileCoords.X(), TileCoords.Y(), CurrentLayer);

        if (SampledTile.IsEmpty())
            return;

        std::optional<Core::uint> TileSheetIdOpt = SampledTile.GetTileSheetId();
        if (!TileSheetIdOpt.has_value())
            return;

        Core::uint TileSheetId = TileSheetIdOpt.value();
        int TileSheetColumns = ScenePtr->GetModel().GetTileSheetColumns(static_cast<int>(TileSheetId));
        if (TileSheetColumns == 0)
            return;

        int Column = SampledTile.GetTileIndex() % TileSheetColumns;
        int Row = SampledTile.GetTileIndex() / TileSheetColumns;

        ScenePtr->GetModel().SelectTile(static_cast<int>(TileSheetId),
            TileRectCoord(TileCoordinate(Column, Row), TileCoordinate(1, 1)));

        ScenePtr->GetModel().SetCurrentTool(EditorTool::Brush);
    }

    void LevelEditorController::FloodFill(WindowCoordinate MousePos)
    {
        std::shared_ptr<CameraComponent> CamPtr = Camera.lock();
        std::shared_ptr<LevelDesignerScene> ScenePtr = Scene.lock();
        std::shared_ptr<CoordinateProjectionSystem> Projector = GetContext().SystemsRegistry->GetCoreSystem<
            Core::CoordinateProjectionSystem>();

        if (!CamPtr || !ScenePtr || !Projector)
            return;

        std::shared_ptr<TileMapComponent> TileMapPtr = ScenePtr->GetModel().GetSelectedTileMap();
        if (!TileMapPtr)
            return;

        if (!ScenePtr->IsClickInCanvas(MousePos))
            return;

        TileSelection Selection = ScenePtr->GetModel().GetCurrentSelection();
        if (!Selection.IsValid() || Selection.SelectionRect.Width() != 1 || Selection.SelectionRect.Height() != 1)
            return;

        WorldCoordinate WorldCoords = Projector->WindowToWorld(MousePos, CamPtr->GetView());
        WorldCoordinate LocalCoords = TileMapPtr->GetOwner()->WorldToLocal(WorldCoords);
        TileCoordinate StartTileCoords = Projector->WorldToTile(LocalCoords);

        if (!StartTileCoords.IsValid())
            return;

        Core::uint CurrentLayer = ScenePtr->GetModel().GetCurrentLayer();
        Core::TileMap& TileMapData = TileMapPtr->GetTileMap();

        Tile TargetTile = TileMapData.GetTile(StartTileCoords.X(), StartTileCoords.Y(), CurrentLayer);

        int TileSheetColumns = ScenePtr->GetModel().GetTileSheetColumns(Selection.TileSheetIndex.value());
        int ReplacementTileIndex = Selection.GetTileIndex(sf::Vector2i(0, 0), TileSheetColumns);
        Tile ReplacementTile(Selection.TileSheetIndex.value(), ReplacementTileIndex);

        if (TargetTile.GetTileSheetId() == ReplacementTile.GetTileSheetId() &&
            TargetTile.GetTileIndex() == ReplacementTile.GetTileIndex())
            return;

        std::vector<TileCoordinate> Stack;
        Stack.push_back(StartTileCoords);

        while (!Stack.empty())
        {
            TileCoordinate Current = Stack.back();
            Stack.pop_back();

            if (Current.X() >= TileMapData.GetWidth() || Current.Y() >= TileMapData.GetHeight())
                continue;

            Tile CurrentTile = TileMapData.GetTile(Current.X(), Current.Y(), CurrentLayer);

            if (CurrentTile.GetTileSheetId() != TargetTile.GetTileSheetId() ||
                CurrentTile.GetTileIndex() != TargetTile.GetTileIndex())
                continue;

            TileMapData.SetTile(Current.X(), Current.Y(), CurrentLayer, ReplacementTile);

            if (Current.X() > 0)
                Stack.push_back(TileCoordinate(Current.X() - 1, Current.Y()));
            if (Current.X() < TileMapData.GetWidth() - 1)
                Stack.push_back(TileCoordinate(Current.X() + 1, Current.Y()));
            if (Current.Y() > 0)
                Stack.push_back(TileCoordinate(Current.X(), Current.Y() - 1));
            if (Current.Y() < TileMapData.GetHeight() - 1)
                Stack.push_back(TileCoordinate(Current.X(), Current.Y() + 1));
        }
    }
}
