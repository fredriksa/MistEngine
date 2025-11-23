#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "Scene.h"
#include "SystemsRegistry.hpp"
#include "Systems/SceneManagerSystem.h"

namespace Core
{
    class Engine
    {
    public:
        Engine();

        template <typename T>
            requires IsScene<T>
        void SetInitialScene();

        void LoadGlobalAssets();
        void Run();

    private:
        void StartSystems();
        void TickSystems(float DeltaTimeS);
        void RenderSystems();
        void ShutdownSystems();

        template <typename Func>
        void ForEachSystem(Func&& Action);

    private:
        std::shared_ptr<SystemsRegistry> SystemsRegistry;
        std::shared_ptr<EngineContext> Context;
    };

    template <typename T>
        requires IsScene<T>
    void Engine::SetInitialScene()
    {
        SystemsRegistry->GetCoreSystem<SceneManagerSystem>()->Push<T>();
    }

    template <typename Func>
    void Engine::ForEachSystem(Func&& Action)
    {
        for (const std::shared_ptr<CoreSystem>& System : SystemsRegistry->GetCoreSystems())
        {
            Action(System);
        }
    }
}
