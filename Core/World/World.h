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

        void Register(std::shared_ptr<WorldObject> WorldObject);

    private:
        std::shared_ptr<EngineContext> Context;
        std::vector<std::shared_ptr<WorldObject>> WorldObjects;
    };
}
