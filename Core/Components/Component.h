#pragma once

#include <memory>

#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "../World/WorldObject.h"
#include "../ThirdParty/json.hpp"

namespace Core
{
    class Component : public ITickable, public IRenderable
    {
    public:
        Component(const std::shared_ptr<WorldObject>& Owner)
            : OwnerWeak(Owner)
        {
        }

        WorldObject* GetOwner() const
        {
            if (std::shared_ptr<WorldObject> Locked = OwnerWeak.lock())
            {
                return Locked.get();
            }
            return nullptr;
        }

        virtual void Tick(float DeltaTimeS) override
        {
        }

        virtual void Render() override
        {
        }

        virtual void FromJson(const nlohmann::json& Data)
        {
        }

        template <typename T>
            requires IsComponent<T>
        T* GetComponent()
        {
            if (WorldObject* Owner = GetOwner())
            {
                return Owner->GetComponent<T>();
            }
            return nullptr;
        }

    protected:
        const EngineContext& GetContext() { return GetOwner()->GetContext(); }

    private:
        std::weak_ptr<WorldObject> OwnerWeak;
    };
}
