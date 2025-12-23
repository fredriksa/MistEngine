#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>

#include "../Components/ComponentConcept.hpp"

namespace Core
{
    class Component;
    class WorldObject;

    class ComponentManager
    {
    public:
        ComponentManager(WorldObject* Owner);

        void Attach(std::shared_ptr<Component> Component);

        template <typename T>
            requires IsComponent<T>
        T* Add();

        template <class T>
            requires IsComponent<T>
        std::shared_ptr<T> Get() const;

        template <class T>
            requires IsComponent<T>
        bool Remove();

        const std::unordered_map<std::type_index, std::shared_ptr<Component>>& GetAll() const { return TypeToComponent; }

        void Tick(float DeltaTimeS);
        void Render();
        void Start();
        void Shutdown();

    private:
        WorldObject* Owner;
        std::unordered_map<std::type_index, std::shared_ptr<Component>> TypeToComponent;
    };

    template <typename T> requires IsComponent<T>
    std::shared_ptr<T> ComponentManager::Get() const
    {
        auto it = TypeToComponent.find(std::type_index(typeid(T)));
        if (it != TypeToComponent.end())
        {
            return std::static_pointer_cast<T>(it->second);
        }
        return nullptr;
    }

    template <typename T> requires IsComponent<T>
    bool ComponentManager::Remove()
    {
        auto it = TypeToComponent.find(std::type_index(typeid(T)));
        if (it != TypeToComponent.end())
        {
            TypeToComponent.erase(it);
            return true;
        }

        return false;
    }
}
