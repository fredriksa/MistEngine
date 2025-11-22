#include "Scene.h"

#include "Engine.h"

namespace Core
{
    Scene::Scene(std::shared_ptr<EngineContext> Context, std::string Name)
        : Name(std::move(Name))
          , Context(std::move(Context))
    {
    }

    void Scene::Tick(float DeltaTimeS)
    {
        World.Tick(DeltaTimeS);
    }

    void Scene::Render()
    {
        World.Render();
    }

    void Scene::Load()
    {
        OnLoad();
    }
}
