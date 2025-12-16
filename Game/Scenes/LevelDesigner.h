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
    };
}
