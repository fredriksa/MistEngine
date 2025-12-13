#pragma once

#include <memory>

#include "../EngineContext.hpp"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "../World/WorldObject.h"
#include "../ThirdParty/json.hpp"

namespace Core
{
    class Component : public ITickable, public IRenderable
    {
    public:
        Component(const std::shared_ptr<WorldObject>& Owner);

        WorldObject* GetOwner() const;

        virtual void Tick(float DeltaTimeS) override
        {
        }

        virtual void Render() override
        {
        }

        virtual bool Initialize(const nlohmann::json& Data)
        {
            return true;
        }

        template <typename T>
            requires IsComponent<T>
        T* GetComponent()
        {
            if (WorldObject* Owner = GetOwner())
            {
                return Owner->Components().Get<T>();
            }
            return nullptr;
        }

    protected:
        const EngineContext& GetContext() { return GetOwner()->GetContext(); }

    private:
        std::weak_ptr<WorldObject> OwnerWeak;
    };
}
