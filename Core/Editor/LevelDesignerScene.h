#pragma once

#include <SFML/Graphics/Rect.hpp>

#include "../Common.h"
#include "../Coordinates/TypedRect.hpp"
#include "../Coordinates/WindowCoordinate.h"
#include "../Scene/Scene.h"

namespace Core
{
    class TileMapComponent;
}

namespace Core
{
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

    struct TileSelection
    {
        std::optional<Core::uint> TileSheetIndex;
        Core::TileRectCoord SelectionRect;

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

    class LevelDesignerScene : public Core::Scene
    {
    public:
        LevelDesignerScene(std::shared_ptr<Core::EngineContext> InContext);

        virtual void OnLoad() override;
        virtual void PreRender() override;
        virtual void PostRender() override;

        bool IsClickInCanvas(Core::WindowCoordinate MousePos) const;
        TileSelection GetCurrentSelection() const { return CurrentSelection; }
        int GetTileSheetColumns(int TileSheetIndex) const;
        Core::uint GetCurrentLayer() const { return CurrentLayer; }
        void ToggleGrid() { bShowGrid = !bShowGrid; }
        void ZoomIn();
        void ZoomOut();
        void ResetView();
        void SaveScene();
        void SaveSceneAs(const std::string& SceneName);
        Core::Task<> OpenScene(const std::string& SceneName);

        std::shared_ptr<Core::TileMapComponent> GetSelectedTileMap() const;

    private:
        void CreateEditorInfrastructure();
        void ExitToMainMenu();
        void RefreshAvailableScenes();
        void RenderTilePalettePanel();
        void RenderTilePaletteDivider();
        void RenderCanvasArea();
        void RenderCanvasToolbar();
        void RenderLayersPanel();
        void RenderPropertiesPanel();
        void RenderSceneHierarchy();
        void RenderObjectProperties();
        void DrawSceneGrid();

        sf::FloatRect CalculateCanvasRect() const;
        void OnTileSheetChanged(int NewTileSheetIndex);

        // UI State
        float TilePalettePanelWidth = 250.0f;
        float PropertiesPanelWidth = 300.0f;
        bool bTilePaletteFloating = false;
        bool bPropertiesFloating = false;
        bool bLayersPanelOpen = false;
        bool bShowGrid = true;
        Core::uint CurrentLayer = 0;
        std::weak_ptr<Core::WorldObject> SelectedObject;

        // Tile Selection State
        int CurrentTileSheetIndex = 0;
        TileSelection CurrentSelection;
        bool bIsDraggingTileSelection = false;
        int DragStartColumn = -1;
        int DragStartRow = -1;

        mutable sf::FloatRect CanvasRect{{0.0f, 0.0f}, {0.0f, 0.0f}};

        bool bShowSaveAsModal = false;
        char SaveAsSceneNameBuffer[256] = "";
        bool bShowOpenSceneModal = false;
        std::vector<std::string> AvailableScenes;
        int SelectedSceneIndex = -1;
        SceneInfo CurrentScene;
        std::optional<Core::Task<>> LoadingTask;

        bool bShowAddComponentModal = false;
        int SelectedComponentTypeIndex = 0;

        std::string EditingObjectName;
    };
}
