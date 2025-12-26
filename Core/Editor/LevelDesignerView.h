#pragma once

#include <memory>
#include <string>
#include <optional>
#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include "../Async/Task.hpp"

namespace Core
{
    class LevelDesignerViewModel;

    class LevelDesignerView
    {
    public:
        LevelDesignerView(LevelDesignerViewModel& InViewModel);

        void RenderMenuBar(std::optional<Task<>>& LoadingTask);
        void RenderMainContent();
        void DrawSceneGrid();
        void RenderGizmos();
        void CalculateCanvasRect();

        sf::FloatRect GetCanvasRect() const { return CanvasRect; }

    private:
        void RenderTilePalettePanel();
        void RenderTilePaletteDivider();
        void RenderCanvasArea();
        void RenderCanvasToolbar();
        void RenderLayersPanel();
        void RenderPropertiesPanel();
        void RenderSceneHierarchy();
        void RenderObjectProperties();

        void RenderSaveAsModal();
        void RenderOpenSceneModal(std::optional<Task<>>& LoadingTask);

        LevelDesignerViewModel& ViewModel;

        mutable sf::FloatRect CanvasRect{{0.0f, 0.0f}, {0.0f, 0.0f}};

        bool bShowSaveAsModal = false;
        bool bShowOpenSceneModal = false;
        char SaveAsSceneNameBuffer[256] = "";
        std::vector<std::string> AvailableScenes;
        int SelectedSceneIndex = -1;

        bool bIsDraggingTileSelection = false;
        int DragStartColumn = -1;
        int DragStartRow = -1;

        bool bShowAddComponentModal = false;
        int SelectedComponentTypeIndex = 0;
        std::string EditingObjectName;
    };
}
