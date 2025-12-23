#pragma once

#include <SFML/Graphics/Rect.hpp>

#include "../../Core/Common.h"
#include "../../Core/Coordinates/TypedRect.hpp"
#include "../../Core/Coordinates/WindowCoordinate.h"
#include "../../Scene/Scene.h"

namespace Game
{
    struct TileSelection
    {
        std::optional<Core::uint> TileSheetIndex;
        Core::TileRectCoord SelectionRect;

        bool IsValid() const
        {
            return TileSheetIndex && SelectionRect.HasArea();
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
        const TileSelection& GetCurrentSelection() const { return CurrentSelection; }
        int GetTileSheetColumns(int TileSheetIndex) const;

    private:
        void ExitToMainMenu();
        void RenderTilePalettePanel();
        void RenderTilePaletteDivider();
        void RenderCanvasArea();
        void RenderPropertiesPanel();

        sf::FloatRect CalculateCanvasRect() const;
        void OnTileSheetChanged(int NewTileSheetIndex);

        // UI State
        float TilePalettePanelWidth = 250.0f;
        float PropertiesPanelWidth = 300.0f;
        bool bTilePaletteFloating = false;
        bool bPropertiesFloating = false;

        // Tile Selection State
        int CurrentTileSheetIndex = 0;
        TileSelection CurrentSelection;
        bool bIsDraggingTileSelection = false;
        int DragStartColumn = -1;
        int DragStartRow = -1;

        float PlaceholderPosX = 0.0f;
        float PlaceholderPosY = 0.0f;
        float PlaceholderRotation = 0.0f;
        float PlaceholderScaleX = 1.0f;
        float PlaceholderScaleY = 1.0f;

        mutable sf::FloatRect CanvasRect{{0.0f, 0.0f}, {0.0f, 0.0f}};
    };
}
