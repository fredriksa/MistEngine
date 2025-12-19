#pragma once

#include "../../Scene/Scene.h"

namespace Game
{
    class LevelDesignerScene : public Core::Scene
    {
    public:
        LevelDesignerScene(std::shared_ptr<Core::EngineContext> InContext);

        virtual void OnLoad() override;
        virtual void PreRender() override;
        virtual void PostRender() override;

    private:
        void ExitToMainMenu();
        void RenderTilePalettePanel();
        void RenderTilePaletteDivider();
        void RenderCanvasArea();
        void RenderPropertiesPanel();

        // UI State
        float TilePalettePanelWidth = 250.0f;
        float PropertiesPanelWidth = 300.0f;
        bool bTilePaletteFloating = false;
        bool bPropertiesFloating = false;

        // Tile Palette State
        int SelectedTileSheetIndex = 0;
        int SelectedTileIndex = -1; // -1 = no selection

        // Placeholder property values
        float PlaceholderPosX = 0.0f;
        float PlaceholderPosY = 0.0f;
        float PlaceholderRotation = 0.0f;
        float PlaceholderScaleX = 1.0f;
        float PlaceholderScaleY = 1.0f;
    };
}
