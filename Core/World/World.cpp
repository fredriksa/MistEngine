#include "World.h"
#include <algorithm>

namespace Core
{
    World::World(std::shared_ptr<Core::EngineContext> Context)
        : Context(std::move(Context))
    {
    }

    void World::Tick(float DeltaTimeS)
    {
        StartNewComponents();

        for (const std::shared_ptr<WorldObject>& WorldObject : WorldObjects)
        {
            WorldObject->Tick(DeltaTimeS);
        }
    }

    void World::Render()
    {
        for (const std::shared_ptr<WorldObject>& WorldObject : WorldObjects)
        {
            WorldObject->Render();
        }
    }

    std::shared_ptr<WorldObject> World::CreateObject()
    {
        std::shared_ptr<WorldObject> Object = std::make_shared<WorldObject>(Context, this);
        WorldObjects.push_back(Object);
        PendingStartObjects.push_back(Object);
        return Object;
    }

    void World::Register(std::shared_ptr<WorldObject> WorldObject)
    {
        WorldObjects.push_back(WorldObject);
        PendingStartObjects.push_back(WorldObject);
    }

    std::shared_ptr<WorldObject> World::GetObjectByName(const std::string& Name) const
    {
        auto It = NamedObjects.find(Name);
        if (It != NamedObjects.end())
        {
            return It->second;
        }
        return nullptr;
    }

    void World::RegisterObjectName(const std::string& Name, std::shared_ptr<WorldObject> Object)
    {
        NamedObjects[Name] = std::move(Object);
    }

    void World::UnregisterObjectName(const std::string& Name)
    {
        NamedObjects.erase(Name);
    }

    bool World::MoveObjectUp(std::shared_ptr<WorldObject> Object)
    {
        auto It = std::find(WorldObjects.begin(), WorldObjects.end(), Object);
        if (It == WorldObjects.end() || It == WorldObjects.begin())
        {
            return false;
        }

        std::iter_swap(It, It - 1);
        return true;
    }

    bool World::MoveObjectDown(std::shared_ptr<WorldObject> Object)
    {
        auto It = std::find(WorldObjects.begin(), WorldObjects.end(), Object);
        if (It == WorldObjects.end() || It == WorldObjects.end() - 1)
        {
            return false;
        }

        std::iter_swap(It, It + 1);
        return true;
    }

    void World::StartNewComponents()
    {
        if (PendingStartObjects.empty())
            return;

        for (const std::shared_ptr<WorldObject>& Object : PendingStartObjects)
        {
            Object->Components().Start();
        }

        PendingStartObjects.clear();
    }

    nlohmann::json World::ToJson() const
    {
        nlohmann::json SceneJson;
        nlohmann::json ObjectsArray = nlohmann::json::array();

        for (const auto& Obj : WorldObjects)
        {
            if (!Obj)
                continue;

            if (Obj->GetTag() == ObjectTag::Game)
            {
                ObjectsArray.push_back(Obj->ToJson());
            }
        }

        SceneJson["objects"] = ObjectsArray;
        return SceneJson;
    }
}
