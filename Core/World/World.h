#pragma once

#include <memory>
#include "../ThirdParty/json.hpp"

#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "ObjectManager.h"
#include "WorldEnvironment.h"

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

        const WorldEnvironment& GetEnvironment() const { return Environment; }
        WorldEnvironment& GetEnvironment() { return Environment; }
        void SetEnvironment(const WorldEnvironment& Env);

        nlohmann::json ToJson() const;

    private:
        std::shared_ptr<EngineContext> Context;
        ObjectManager ObjectMgr;
        WorldEnvironment Environment;
    };
}
