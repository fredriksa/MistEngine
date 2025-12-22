#include "World.h"

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
}
