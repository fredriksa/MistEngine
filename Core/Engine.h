#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "IEngine.hpp"
#include "Scene/Scene.h"
#include "SystemsRegistry.hpp"
#include "Systems/SceneManagerSystem.h"

namespace Core
{
    class Engine : public IEngine
    {
    public:
        Engine();

        template <typename T>
            requires IsScene<T>
        void SetInitialScene();

        void Run();

        virtual void Shutdown() override;

    private:
        template <typename Func>
        void ForEachSystem(Func&& Action);

    private:
        std::shared_ptr<SystemsRegistry> SystemsRegistry;
        std::shared_ptr<EngineContext> Context;
        bool bPendingShutdown = false;
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
