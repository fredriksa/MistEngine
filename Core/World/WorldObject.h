#pragma once

#include <memory>
#include <typeindex>

#include <type_traits>
#include <unordered_map>

#include "Transform.hpp"
#include "../Components/ComponentConcept.hpp"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"

namespace Core
{
    class Component;
}

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

        template <typename T>
            requires IsComponent<T>
        T* AddComponent();

        template <class T>
            requires IsComponent<T>
        std::shared_ptr<T> GetComponent();

        template <class T>
            requires IsComponent<T>
        bool RemoveComponent();

        const EngineContext& GetContext();

    private:
        std::shared_ptr<EngineContext> Context;
        Transform Transform;
        std::unordered_map<std::type_index, std::shared_ptr<Component>> TypeToComponent;
    };

    template <typename T> requires IsComponent<T>
    T* WorldObject::AddComponent()
    {
        std::shared_ptr<T> Component = std::make_shared<T>(this);
        TypeToComponent.emplace(std::type_index(typeid(T)), Component);
        return Component.get();
    }

    template <typename T> requires IsComponent<T>
    std::shared_ptr<T> WorldObject::GetComponent()
    {
        auto it = TypeToComponent.find(std::type_index(typeid(T)));
        if (it != TypeToComponent.end())
        {
            return std::static_pointer_cast<T>(it->second.get());
        }
        return nullptr;
    }

    template <typename T> requires IsComponent<T>
    bool WorldObject::RemoveComponent()
    {
        auto it = TypeToComponent.find(std::type_index(typeid(T)));
        if (it != TypeToComponent.end())
        {
            TypeToComponent.erase(it);
            return true;
        }

        return false;
    }

    template <typename T>
    concept IsWorldObject = std::is_base_of_v<WorldObject, T>;
}
