#pragma once
#include "../../Core/Scene/Scene.h"

namespace Game
{
    class MainMenuScene : public Core::Scene
    {
    public:
        MainMenuScene(std::shared_ptr<Core::EngineContext> InContext);

        virtual void OnLoad() override;
        virtual void PreRender() override;
        virtual void PostRender() override;

    private:
        void EnterLevelDesigner();
        void Shutdown();
    };
}
