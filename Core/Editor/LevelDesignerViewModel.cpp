#include "LevelDesignerViewModel.h"
#include "LevelDesignerModel.h"
#include "../World/World.h"
#include "../World/WorldObject.h"
#include "../Components/CameraComponent.h"
#include "../Components/TileMapComponent.h"
#include "../Components/TransformComponent.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/SceneManagerSystem.h"
#include "../Systems/AssetRegistrySystem.h"
#include "../Systems/CoordinateProjectionSystem.h"
#include "../TileMap/TileSheet.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace Core
{
    LevelDesignerViewModel::LevelDesignerViewModel(LevelDesignerModel& InModel)
        : Model(InModel)
    {
    }

    void LevelDesignerViewModel::SelectObject(const std::shared_ptr<WorldObject>& Object)
    {
        Model.GetSelection().Clear();
        if (Object)
        {
            Model.GetSelection().Add(Object);
        }
    }

    std::shared_ptr<WorldObject> LevelDesignerViewModel::GetSelectedObject() const
    {
        return Model.GetPrimarySelected();
    }

    bool LevelDesignerViewModel::IsObjectSelected(const WorldObject* Object) const
    {
        return Model.GetSelection().Contains(Object);
    }

    bool LevelDesignerViewModel::IsGridVisible() const
    {
        return Model.IsGridVisible();
    }

    void LevelDesignerViewModel::ToggleGrid()
    {
        Model.ToggleGrid();
    }

    std::string LevelDesignerViewModel::GetObjectDisplayName(const WorldObject* Object) const
    {
        if (!Object)
            return "[Null]";

        return Object->GetName().empty() ? "[Unnamed]" : Object->GetName();
    }

    std::shared_ptr<TileMapComponent> LevelDesignerViewModel::GetSelectedTileMap() const
    {
        return Model.GetSelectedTileMap();
    }

    void LevelDesignerViewModel::SelectTile(int TileSheetIndex, TileRectCoord Rect)
    {
        Model.SelectTile(TileSheetIndex, Rect);
    }

    TileSelection LevelDesignerViewModel::GetCurrentSelection() const
    {
        return Model.GetCurrentSelection();
    }

    int LevelDesignerViewModel::GetCurrentTileSheetIndex() const
    {
        return Model.GetCurrentTileSheetIndex();
    }

    void LevelDesignerViewModel::SetTileSheetIndex(int Index)
    {
        Model.SetTileSheetIndex(Index);
    }

    int LevelDesignerViewModel::GetTileSheetColumns(int TileSheetIndex) const
    {
        return Model.GetTileSheetColumns(TileSheetIndex);
    }

    uint LevelDesignerViewModel::GetCurrentLayer() const
    {
        return Model.GetCurrentLayer();
    }

    void LevelDesignerViewModel::SetCurrentLayer(uint Layer)
    {
        Model.SetCurrentLayer(Layer);
    }

    EditorTool LevelDesignerViewModel::GetCurrentTool() const
    {
        return Model.GetCurrentTool();
    }

    void LevelDesignerViewModel::SetCurrentTool(EditorTool Tool)
    {
        Model.SetCurrentTool(Tool);
    }

    void LevelDesignerViewModel::ToggleTool(EditorTool Tool)
    {
        if (Model.GetCurrentTool() == Tool)
            Model.SetCurrentTool(EditorTool::Select);
        else
            Model.SetCurrentTool(Tool);
    }

    void LevelDesignerViewModel::NewScene()
    {
        Model.NewScene();
    }

    void LevelDesignerViewModel::SaveScene()
    {
        Model.SaveScene();
    }

    void LevelDesignerViewModel::SaveSceneAs(const std::string& SceneName)
    {
        Model.SaveSceneAs(SceneName);
    }

    Task<void> LevelDesignerViewModel::LoadScene(const std::string& SceneName)
    {
        return Model.LoadScene(SceneName);
    }

    void LevelDesignerViewModel::SetMouseOverBlockingUI(bool bBlocking)
    {
        Model.SetMouseOverBlockingUI(bBlocking);
    }

    void LevelDesignerViewModel::StartTimePreview()
    {
        Model.StartTimePreview();
    }

    void LevelDesignerViewModel::StopTimePreview()
    {
        Model.StopTimePreview();
    }

    void LevelDesignerViewModel::UpdateTimePreview()
    {
        Model.UpdateTimePreview();
    }

    void LevelDesignerViewModel::SetTimePreviewDuration(float Duration)
    {
        Model.SetTimePreviewDuration(Duration);
    }

    World& LevelDesignerViewModel::GetWorld()
    {
        return Model.GetWorld();
    }

    const World& LevelDesignerViewModel::GetWorld() const
    {
        return Model.GetWorld();
    }

    std::shared_ptr<WorldObject> LevelDesignerViewModel::CreateNewObject()
    {
        std::shared_ptr<WorldObject> NewObject = Model.CreateObject();
        NewObject->SetTag(ObjectTag::Game);
        NewObject->SetName("New Object");
        NewObject->Components().Add<TransformComponent>();

        SelectObject(NewObject);

        return NewObject;
    }

    void LevelDesignerViewModel::DeleteSelectedObject()
    {
        const std::shared_ptr<WorldObject> Selected = Model.GetPrimarySelected();
        if (Selected)
        {
            Model.RemoveObject(Selected);
            Model.GetSelection().Clear();
        }
    }

    void LevelDesignerViewModel::MoveSelectedObjectUp()
    {
        const std::shared_ptr<WorldObject> Selected = Model.GetPrimarySelected();
        if (Selected)
        {
            Model.MoveObjectUp(Selected);
        }
    }

    void LevelDesignerViewModel::MoveSelectedObjectDown()
    {
        const std::shared_ptr<WorldObject> Selected = Model.GetPrimarySelected();
        if (Selected)
        {
            Model.MoveObjectDown(Selected);
        }
    }

    std::vector<std::shared_ptr<WorldObject>> LevelDesignerViewModel::GetGameObjects() const
    {
        std::vector<std::shared_ptr<WorldObject>> GameObjects;

        for (const std::shared_ptr<WorldObject>& Obj : Model.GetAllObjects())
        {
            if (Obj && Obj->GetTag() == ObjectTag::Game)
            {
                GameObjects.push_back(Obj);
            }
        }

        return GameObjects;
    }

    std::vector<std::string> LevelDesignerViewModel::GetAvailableScenes() const
    {
        return Model.GetAvailableScenes();
    }

    bool LevelDesignerViewModel::CanPlayTest() const
    {
        return Model.CanPlayTest();
    }

    void LevelDesignerViewModel::RequestExitToMainMenu()
    {
        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = Model.GetAllObjects();
        if (AllObjects.empty())
            return;

        const std::shared_ptr<EngineContext> Context = AllObjects[0]->GetContextPtr();
        if (!Context || !Context->SystemsRegistry)
            return;

        const std::shared_ptr<SceneManagerSystem> SceneManager =
            Context->SystemsRegistry->GetCoreSystem<SceneManagerSystem>();
        if (SceneManager)
        {
            SceneManager->RequestPop();
        }
    }

    std::vector<std::shared_ptr<const TileSheet>> LevelDesignerViewModel::GetAllTileSheets() const
    {
        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = Model.GetAllObjects();
        if (AllObjects.empty())
            return {};

        const std::shared_ptr<EngineContext> Context = AllObjects[0]->GetContextPtr();
        if (!Context || !Context->SystemsRegistry)
            return {};

        const std::shared_ptr<AssetRegistrySystem> AssetRegistry =
            Context->SystemsRegistry->GetCoreSystem<AssetRegistrySystem>();
        if (!AssetRegistry)
            return {};

        return AssetRegistry->GetAllTileSheets();
    }

    sf::RenderTarget& LevelDesignerViewModel::GetRenderer() const
    {
        return *Model.GetContext()->Renderer;
    }

    sf::Vector2u LevelDesignerViewModel::GetWindowSize() const
    {
        return Model.GetContext()->WindowSize;
    }

    bool LevelDesignerViewModel::IsSelectingRectangle() const
    {
        return Model.IsSelectingRectangle();
    }

    WindowCoordinate LevelDesignerViewModel::GetSelectionRectStart() const
    {
        return Model.GetSelectionRectStart();
    }

    WindowCoordinate LevelDesignerViewModel::GetSelectionRectCurrent() const
    {
        return Model.GetSelectionRectCurrent();
    }

    std::vector<WorldObject*> LevelDesignerViewModel::GetObjectsInCurrentSelectionRectangle() const
    {
        if (!Model.IsSelectingRectangle())
            return {};

        const std::shared_ptr<WorldObject> CameraObject = Model.GetWorld().Objects().GetByName("EditorCamera");
        if (!CameraObject)
            return {};

        const std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>();
        if (!Camera)
            return {};

        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = Model.GetAllObjects();
        if (AllObjects.empty())
            return {};

        const std::shared_ptr<EngineContext> Context = AllObjects[0]->GetContextPtr();
        if (!Context || !Context->SystemsRegistry)
            return {};

        const std::shared_ptr<CoordinateProjectionSystem> Projector =
            Context->SystemsRegistry->GetCoreSystem<CoordinateProjectionSystem>();
        if (!Projector)
            return {};

        const WindowCoordinate SelectStart = Model.GetSelectionRectStart();
        const WindowCoordinate SelectCurrent = Model.GetSelectionRectCurrent();

        const WorldCoordinate WorldStart = Projector->WindowToWorld(SelectStart, Camera->GetView());
        const WorldCoordinate WorldEnd = Projector->WindowToWorld(SelectCurrent, Camera->GetView());

        const float MinX = std::min(WorldStart.X(), WorldEnd.X());
        const float MaxX = std::max(WorldStart.X(), WorldEnd.X());
        const float MinY = std::min(WorldStart.Y(), WorldEnd.Y());
        const float MaxY = std::max(WorldStart.Y(), WorldEnd.Y());

        const WorldCoordinate TopLeft(MinX, MinY);
        const WorldCoordinate BottomRight(MaxX, MaxY);

        return Model.GetObjectsInRectangle(TopLeft, BottomRight);
    }
}
