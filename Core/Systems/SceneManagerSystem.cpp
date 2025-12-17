#include "SceneManagerSystem.h"

namespace Core
{
    SceneManagerSystem::SceneManagerSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("SceneManager", Type, std::move(InContext))
    {
    }

    bool SceneManagerSystem::Pop()
    {
        if (Scenes.empty())
        {
            return false;
        }

        if (ActiveScene)
        {
            ActiveScene->Exit();
        }

        Scenes.pop();

        if (Scenes.empty())
        {
            ActiveScene.reset();
        }
        else
        {
            ActiveScene = Scenes.top();
        }

        return true;
    }

    void SceneManagerSystem::RequestPop()
    {
        bPopRequested = true;
    }

    void SceneManagerSystem::Start()
    {
    }

    void SceneManagerSystem::Tick(float DeltaTimeS)
    {
        if (bPopRequested)
        {
            bPopRequested = false;
            Pop();
            return;
        }

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
