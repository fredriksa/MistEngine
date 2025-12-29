#include "World.h"

#include <algorithm>
#include "WorldObject.h"

namespace Core
{
    World::World(std::shared_ptr<Core::EngineContext> Context)
        : Context(Context), ObjectMgr(this, Context)
    {
    }

    void World::Tick(float DeltaTimeS)
    {
        Environment.GetTime().Tick(DeltaTimeS);

        ObjectMgr.StartPendingComponents();

        for (const std::shared_ptr<WorldObject>& Object : ObjectMgr.GetAll())
        {
            Object->Tick(DeltaTimeS);
        }
    }

    void World::Render()
    {
        for (const std::shared_ptr<WorldObject>& Object : ObjectMgr.GetAll())
        {
            Object->Render();
        }
    }

    nlohmann::json World::ToJson() const
    {
        nlohmann::json SceneJson = ObjectMgr.ToJson();
        SceneJson["worldEnvironment"] = Environment.ToJson();
        return SceneJson;
    }

    void World::SetEnvironment(const WorldEnvironment& Env)
    {
        Environment = Env;
    }
}
