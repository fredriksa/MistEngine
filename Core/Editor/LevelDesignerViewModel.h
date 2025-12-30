#pragma once

#include <memory>
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "../Common.h"
#include "../Coordinates/TypedRect.hpp"
#include "../Coordinates/WindowCoordinate.h"

namespace sf
{
    class RenderWindow;
}

namespace Core
{
    class LevelDesignerModel;
    class WorldObject;
    class TileMapComponent;
    class TileSheet;
    class World;
    class ObjectSelection;
    struct TileSelection;
    struct EngineContext;
    class SceneInfo;
    enum class EditorTool;
    template<typename T> class Task;

    class LevelDesignerViewModel
    {
    public:
        LevelDesignerViewModel(LevelDesignerModel& InModel);

        void SelectObject(const std::shared_ptr<WorldObject>& Object);
        std::shared_ptr<WorldObject> GetSelectedObject() const;
        bool IsObjectSelected(const WorldObject* Object) const;
        std::string GetObjectDisplayName(const WorldObject* Object) const;
        std::shared_ptr<TileMapComponent> GetSelectedTileMap() const;

        void SelectTile(int TileSheetIndex, TileRectCoord Rect);
        TileSelection GetCurrentSelection() const;
        int GetCurrentTileSheetIndex() const;
        void SetTileSheetIndex(int Index);
        int GetTileSheetColumns(int TileSheetIndex) const;

        uint GetCurrentLayer() const;
        void SetCurrentLayer(uint Layer);

        EditorTool GetCurrentTool() const;
        void SetCurrentTool(EditorTool Tool);
        void ToggleTool(EditorTool Tool);

        void NewScene();
        void SaveScene();
        void SaveSceneAs(const std::string& SceneName);
        Task<void> LoadScene(const std::string& SceneName);

        void SetMouseOverBlockingUI(bool bBlocking);

        void StartTimePreview();
        void StopTimePreview();
        void UpdateTimePreview();
        void SetTimePreviewDuration(float Duration);

        World& GetWorld();
        const World& GetWorld() const;

        float GetTilePalettePanelWidth() const { return TilePalettePanelWidth; }
        void SetTilePalettePanelWidth(float Width) { TilePalettePanelWidth = Width; }

        float GetPropertiesPanelWidth() const { return PropertiesPanelWidth; }
        void SetPropertiesPanelWidth(float Width) { PropertiesPanelWidth = Width; }

        bool IsGridVisible() const;
        void ToggleGrid();

        bool IsSelectingRectangle() const;
        struct WindowCoordinate GetSelectionRectStart() const;
        struct WindowCoordinate GetSelectionRectCurrent() const;
        std::vector<WorldObject*> GetObjectsInCurrentSelectionRectangle() const;

        bool IsLayersPanelOpen() const { return bLayersPanelOpen; }
        void SetLayersPanelOpen(bool bOpen) { bLayersPanelOpen = bOpen; }

        std::shared_ptr<WorldObject> CreateNewObject();
        void DeleteSelectedObject();
        void MoveSelectedObjectUp();
        void MoveSelectedObjectDown();

        std::vector<std::shared_ptr<WorldObject>> GetGameObjects() const;
        std::vector<std::string> GetAvailableScenes() const;
        bool CanPlayTest() const;

        void RequestExitToMainMenu();
        std::vector<std::shared_ptr<const TileSheet>> GetAllTileSheets() const;
        sf::RenderTarget& GetRenderer() const;
        sf::Vector2u GetWindowSize() const;

        const LevelDesignerModel& GetModel() const { return Model; }

    private:
        LevelDesignerModel& Model;

        float TilePalettePanelWidth = 250.0f;
        float PropertiesPanelWidth = 300.0f;
        bool bLayersPanelOpen = false;
    };
}
