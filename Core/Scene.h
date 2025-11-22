#pragma once

#include <iostream>

#include "EngineContext.hpp"
#include "World/World.h"
#include "Interfaces/IRenderable.hpp"
#include "Interfaces/ITickable.hpp"

namespace Core
{
    class Scene : public ITickable, public IRenderable
    {
    public:
        Scene(std::shared_ptr<EngineContext> Context, std::string Name);
        virtual ~Scene() = default;
        const std::string& GetName() { return Name; }

        void Tick(float DeltaTimeS) final;
        void Render() final;
        void Load();

    protected:
        virtual void OnLoad()
        {
        };

    protected:
        std::string Name;
        std::shared_ptr<EngineContext> Context;
        World World;
    };

    template <typename T>
    concept IsScene = std::is_base_of_v<Scene, T>;
}
