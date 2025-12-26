#pragma once

#include <memory>
#include "../ThirdParty/json.hpp"

#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "ObjectManager.h"

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

        ObjectManager& Objects() { return ObjectMgr; }
        const ObjectManager& Objects() const { return ObjectMgr; }

        nlohmann::json ToJson() const;

    private:
        std::shared_ptr<EngineContext> Context;
        ObjectManager ObjectMgr;
    };
}
