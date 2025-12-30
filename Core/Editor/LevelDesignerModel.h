#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include "../Async/Task.hpp"
#include "../Common.h"
#include "../Coordinates/TypedRect.hpp"
#include "../Coordinates/WorldCoordinate.h"
#include "../Coordinates/WindowCoordinate.h"
#include "ObjectSelection.h"

namespace Core
{
    class World;
    class WorldObject;
    class TileMapComponent;
    struct EngineContext;

    enum class EditorTool
    {
        Select,
        Brush,
        Fill,
        Eraser,
        Eyedropper
    };

    enum class GizmoPart
    {
        None,
        CenterHandle,
        XAxis,
        YAxis
    };

    enum class DragMode
    {
        None,
        XY,
        XOnly,
        YOnly
    };

    struct TileSelection
    {
        std::optional<uint> TileSheetIndex;
        TileRectCoord SelectionRect;

        bool IsValid() const
        {
            return TileSheetIndex.has_value() && SelectionRect.HasArea();
        }

        int GetTileIndex(sf::Vector2i Offset, int TileSheetColumns) const
        {
            const int Column = SelectionRect.Position.X() + Offset.x;
            const int Row = SelectionRect.Position.Y() + Offset.y;
            return Row * TileSheetColumns + Column;
        }
    };

    class SceneInfo
    {
    public:
        void SetPath(const std::string& InPath);
        const std::string& GetName() const;
        const std::string& GetPath() const;
        bool IsValid() const;
        void Clear();

    private:
        std::string Name;
        std::string Path;
    };

    class LevelDesignerModel
    {
    public:
        LevelDesignerModel(World& InWorld, std::shared_ptr<EngineContext> InContext);

        std::shared_ptr<WorldObject> CreateObject();
        void RemoveObject(const std::shared_ptr<WorldObject>& Object);
        bool MoveObjectUp(const std::shared_ptr<WorldObject>& Object);
        bool MoveObjectDown(const std::shared_ptr<WorldObject>& Object);
        const std::vector<std::shared_ptr<WorldObject>>& GetAllObjects() const;

        void SaveScene();
        void SaveSceneAs(const std::string& SceneName);
        Task<> LoadScene(const std::string& SceneName);
        void NewScene();

        const SceneInfo& GetCurrentScene() const { return CurrentScene; }
        std::vector<std::string> GetAvailableScenes() const;
        bool CanPlayTest() const;

        void SelectTile(int TileSheetIndex, TileRectCoord Rect);
        TileSelection GetCurrentSelection() const { return CurrentSelection; }
        int GetCurrentTileSheetIndex() const { return CurrentTileSheetIndex; }
        void SetTileSheetIndex(int Index);
        int GetTileSheetColumns(int TileSheetIndex) const;

        uint GetCurrentLayer() const { return CurrentLayer; }
        void SetCurrentLayer(uint Layer) { CurrentLayer = Layer; }

        bool IsGridVisible() const { return bShowGrid; }
        void ToggleGrid() { bShowGrid = !bShowGrid; }
        void SetGridVisible(bool bVisible) { bShowGrid = bVisible; }

        ObjectSelection& GetSelection() { return Selection; }
        const ObjectSelection& GetSelection() const { return Selection; }
        std::shared_ptr<WorldObject> GetPrimarySelected() const { return Selection.GetPrimary(); }
        std::shared_ptr<TileMapComponent> GetSelectedTileMap() const;

        void SetCurrentTool(EditorTool Tool) { CurrentTool = Tool; }
        EditorTool GetCurrentTool() const { return CurrentTool; }

        void SelectObjectAtPosition(struct WorldCoordinate WorldPos, bool bAdditive);
        void SelectObjectsInRectangle(WorldCoordinate TopLeft, WorldCoordinate BottomRight, bool bAdditive);
        void ClearSelection();

        void SetHoveredObject(WorldCoordinate WorldPos);
        void ClearHoveredObject();
        WorldObject* GetHoveredObject() const { return HoveredObject; }

        void StartSelectionRectangle(struct WindowCoordinate Start);
        void UpdateSelectionRectangle(struct WindowCoordinate Current);
        void ClearSelectionRectangle();
        bool IsSelectingRectangle() const { return bIsSelectingRectangle; }
        WindowCoordinate GetSelectionRectStart() const { return SelectRectStart; }
        WindowCoordinate GetSelectionRectCurrent() const { return SelectRectCurrent; }

        GizmoPart GetGizmoPartAtPosition(WorldCoordinate WorldPos) const;
        void StartDraggingObjects(WorldCoordinate StartPos, DragMode Mode);
        void UpdateDraggedObjects(WorldCoordinate CurrentPos);
        void EndDraggingObjects();
        bool IsDraggingObjects() const { return bIsDraggingObjects; }

        WorldObject* GetObjectAtPosition(WorldCoordinate WorldPos) const;
        std::vector<WorldObject*> GetObjectsInRectangle(WorldCoordinate TopLeft, WorldCoordinate BottomRight) const;

        World& GetWorld() { return WorldRef; }
        const World& GetWorld() const { return WorldRef; }

        std::shared_ptr<EngineContext> GetContext() const { return Context; }

        void SetMouseOverBlockingUI(bool bBlocking) { bMouseOverBlockingUI = bBlocking; }
        bool IsMouseOverBlockingUI() const { return bMouseOverBlockingUI; }

        void StartTimePreview();
        void StopTimePreview();
        void UpdateTimePreview();
        bool IsPreviewingTime() const { return bIsPreviewingTime; }
        float GetTimePreviewProgress() const;

        void SetTimePreviewDuration(float Duration) { TimePreviewDuration = Duration; }
        float GetTimePreviewDuration() const { return TimePreviewDuration; }

    private:
        sf::FloatRect GetObjectBounds(const WorldObject* Object) const;

        World& WorldRef;
        std::shared_ptr<EngineContext> Context;
        SceneInfo CurrentScene;

        TileSelection CurrentSelection;
        int CurrentTileSheetIndex = 0;
        uint CurrentLayer = 0;
        bool bShowGrid = true;
        ObjectSelection Selection;
        EditorTool CurrentTool = EditorTool::Select;
        WorldObject* HoveredObject = nullptr;
        bool bIsSelectingRectangle = false;
        WindowCoordinate SelectRectStart{0, 0};
        WindowCoordinate SelectRectCurrent{0, 0};
        bool bIsDraggingObjects = false;
        DragMode CurrentDragMode = DragMode::None;
        WorldCoordinate DragStartPos;
        std::vector<std::pair<WorldObject*, sf::Vector2f>> DraggedObjectsInitialPositions;
        bool bMouseOverBlockingUI = false;

        bool bIsPreviewingTime = false;
        std::chrono::steady_clock::time_point TimePreviewStart;
        int SavedPreviewHours = 0;
        int SavedPreviewMinutes = 0;
        float TimePreviewDuration = 15.0f;
    };
}
