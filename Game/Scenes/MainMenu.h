#pragma once
#include "../../Core/Scene.h"

namespace Game
{
    class MainMenuScene : public Core::Scene
    {
    public:
        MainMenuScene(std::shared_ptr<Core::EngineContext> InContext);

        virtual void OnLoad() override;
    };
}
