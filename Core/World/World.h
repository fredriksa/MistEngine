#pragma once

#include <memory>
#include <vector>

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

        template <typename T>
            requires IsWorldObject<T>
        const std::shared_ptr<WorldObject>& CreateWorldObject();

    private:
        std::shared_ptr<EngineContext> Context;
        std::vector<std::shared_ptr<WorldObject>> WorldObjects;
    };

    template <typename T> requires IsWorldObject<T>
    const std::shared_ptr<WorldObject>& World::CreateWorldObject()
    {
        WorldObjects.push_back(std::make_shared<WorldObject>(Context));
        return WorldObjects.back();
    }
}
