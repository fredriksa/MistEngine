#pragma once

#include <memory>

#include "ComponentManager.h"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"

namespace Core
{
    struct EngineContext;
    class TransformComponent;

    class WorldObject : public ITickable, public IRenderable, public std::enable_shared_from_this<WorldObject>
    {
    public:
        WorldObject(std::shared_ptr<EngineContext> Context);

        void Tick(float DeltaTimeS) final;
        void Render() final;

        ComponentManager& Components() { return ComponentsMgr; }
        TransformComponent* Transform();

        const EngineContext& GetContext() const;

    private:
        std::shared_ptr<EngineContext> Context;
        ComponentManager ComponentsMgr{this};
    };

    template <typename T>
    concept IsWorldObject = std::is_base_of_v<WorldObject, T>;

    template <typename T> requires IsComponent<T>
    T* ComponentManager::Add()
    {
        std::shared_ptr<T> Component = std::make_shared<T>(Owner->shared_from_this());
        TypeToComponent.emplace(std::type_index(typeid(T)), Component);
        return Component.get();
    }
}
