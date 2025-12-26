#pragma once

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "../Async/Task.hpp"
#include "../Common.h"
#include "../Coordinates/TypedRect.hpp"

namespace Core
{
    class World;
    class WorldObject;
    class TileMapComponent;
    struct EngineContext;

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

        void SetSelectedObject(const std::shared_ptr<WorldObject>& Object);
        std::shared_ptr<WorldObject> GetSelectedObject() const;
        std::shared_ptr<TileMapComponent> GetSelectedTileMap() const;

        World& GetWorld() { return WorldRef; }
        const World& GetWorld() const { return WorldRef; }

    private:
        World& WorldRef;
        std::shared_ptr<EngineContext> Context;
        SceneInfo CurrentScene;

        TileSelection CurrentSelection;
        int CurrentTileSheetIndex = 0;
        uint CurrentLayer = 0;
        bool bShowGrid = true;
        std::weak_ptr<WorldObject> SelectedObject;
    };
}
