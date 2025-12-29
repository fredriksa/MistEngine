#pragma once

#include "../Scene/Scene.h"
#include "../Coordinates/WindowCoordinate.h"
#include "LevelDesignerModel.h"
#include "LevelDesignerViewModel.h"
#include "LevelDesignerView.h"
#include <optional>

namespace Core
{
    class LevelDesignerScene : public Scene
    {
    public:
        LevelDesignerScene(std::shared_ptr<EngineContext> InContext);

        void OnLoad() override;
        void PreRender() override;
        void RenderUI() override;

        bool IsClickInCanvas(WindowCoordinate MousePos) const;
        void ZoomIn();
        void ZoomOut();
        void ResetView();

        LevelDesignerModel& GetModel() { return Model; }
        const LevelDesignerModel& GetModel() const { return Model; }

    private:
        void CreateEditorInfrastructure();

        LevelDesignerModel Model;
        LevelDesignerViewModel ViewModel;
        LevelDesignerView View;

        std::optional<Task<>> LoadingTask;
    };
}
