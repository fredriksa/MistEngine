#pragma once

#include <string>
#include <memory>

#include "CoreSystems.h"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"

namespace Core
{
    struct EngineContext;

    class CoreSystem : public ITickable, public IRenderable
    {
    public:
        CoreSystem(std::string Name, ECoreSystemType Type, std::shared_ptr<EngineContext> InContext)
            : Name(std::move(Name))
              , Type(Type)
              , Context(std::move(InContext))
        {
        }

        virtual ~CoreSystem() = default;

        const std::string& GetName() const { return Name; }
        ECoreSystemType GetType() const { return Type; }

        virtual void Start()
        {
        }

        virtual void Tick(float DeltaTimeS) override
        {
        }

        virtual void Render() override
        {
        }

        virtual void Shutdown()
        {
        }

        virtual void RenderUI()
        {
        }

    protected:
        std::shared_ptr<EngineContext>& GetContext()
        {
            return Context;
        }

        const std::shared_ptr<EngineContext>& GetContext() const
        {
            return Context;
        }

    private:
        std::string Name;
        ECoreSystemType Type;
        std::shared_ptr<EngineContext> Context;
    };

    template <typename T>
    concept IsCoreSystem = std::is_base_of_v<CoreSystem, T>;
}
