#pragma once

#include <memory>
#include <string>
#include "../Thirdparty/json.hpp"

#include "ComponentManager.h"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "../Coordinates/WorldCoordinate.h"

namespace Core
{
    struct EngineContext;
    class TransformComponent;
    class World;

    enum class ObjectTag
    {
        Game,      // Serialized - part of saved level
        Editor,    // Not serialized - editor infrastructure
        Runtime    // Not serialized - temporary runtime objects
    };

    class WorldObject : public ITickable, public IRenderable, public std::enable_shared_from_this<WorldObject>
    {
    public:
        WorldObject(std::shared_ptr<EngineContext> Context, World* InWorld);

        void Tick(float DeltaTimeS) final;
        void Render() final;

        ComponentManager& Components() { return ComponentsMgr; }
        TransformComponent* Transform();

        const EngineContext& GetContext() const;
        std::shared_ptr<EngineContext> GetContextPtr() const { return Context; }
        World* GetWorld() const { return OwningWorld; }

        void SetName(const std::string& InName);
        const std::string& GetName() const { return Name; }

        void SetTag(ObjectTag InTag) { Tag = InTag; }
        ObjectTag GetTag() const { return Tag; }

        WorldCoordinate WorldToLocal(const WorldCoordinate& WorldPos) const;
        WorldCoordinate LocalToWorld(const WorldCoordinate& LocalPos) const;

        nlohmann::json ToJson() const;

    private:
        std::shared_ptr<EngineContext> Context;
        World* OwningWorld;
        ComponentManager ComponentsMgr{this};
        std::string Name;
        ObjectTag Tag = ObjectTag::Game;
    };

    template <typename T>
    concept IsWorldObject = std::is_base_of_v<WorldObject, T>;

    template <typename T> requires IsComponent<T>
    T* ComponentManager::Add()
    {
        std::shared_ptr<T> Component = std::make_shared<T>(Owner->shared_from_this(), Owner->GetContextPtr());
        TypeToComponent.emplace(std::type_index(typeid(T)), Component);
        return Component.get();
    }
}
