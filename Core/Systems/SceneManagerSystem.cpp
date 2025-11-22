#include "SceneManagerSystem.h"

namespace Core
{
    SceneManagerSystem::SceneManagerSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("SceneManager", Type, std::move(InContext))
    {
    }

    bool SceneManagerSystem::Pop()
    {
        bool bDidPop = false;
        if (!Scenes.empty())
        {
            Scenes.pop();
            bDidPop = true;
        }

        if (bDidPop)
        {
            if (Scenes.empty())
            {
                ActiveScene.reset();
            }
            else
            {
                ActiveScene = Scenes.top();
            }
        }

        return bDidPop;
    }

    void SceneManagerSystem::Start()
    {
        if (ActiveScene)
        {
            ActiveScene->Load();
        }
    }

    void SceneManagerSystem::Tick(float DeltaTimeS)
    {
        if (ActiveScene)
        {
            ActiveScene->Tick(DeltaTimeS);
        }
    }

    void SceneManagerSystem::Render()
    {
        if (ActiveScene)
        {
            ActiveScene->Render();
        }
    }
}
