#pragma once

#include <memory>

#include "Transform.hpp"
#include "ComponentManager.h"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"

namespace Core
{
    struct EngineContext;

    class WorldObject : public ITickable, public IRenderable
    {
    public:
        WorldObject(std::shared_ptr<EngineContext> Context);
        Transform& GetTransform() { return Transform; }

        void Tick(float DeltaTimeS) final;
        void Render() final;

        ComponentManager& Components() { return ComponentsMgr; }

        const EngineContext& GetContext() const;

    private:
        std::shared_ptr<EngineContext> Context;
        Transform Transform;
        ComponentManager ComponentsMgr{this};
    };

    template <typename T>
    concept IsWorldObject = std::is_base_of_v<WorldObject, T>;
}
