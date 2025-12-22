#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "WorldObject.h"

namespace Core
{
    struct EngineContext;
}

namespace Core
{
    class World : public ITickable, public IRenderable
    {
    public:
        World(std::shared_ptr<EngineContext> Context);
        void Tick(float DeltaTimeS) override;
        void Render() override;

        std::shared_ptr<WorldObject> CreateObject();
        void Register(std::shared_ptr<WorldObject> WorldObject);

        std::shared_ptr<WorldObject> GetObjectByName(const std::string& Name) const;

        void RegisterObjectName(const std::string& Name, std::shared_ptr<WorldObject> Object);
        void UnregisterObjectName(const std::string& Name);

    private:
        void StartNewComponents();

        std::shared_ptr<EngineContext> Context;
        std::vector<std::shared_ptr<WorldObject>> WorldObjects;
        std::unordered_map<std::string, std::shared_ptr<WorldObject>> NamedObjects;
        std::vector<std::shared_ptr<WorldObject>> PendingStartObjects;
    };
}
