#pragma once
#include <stack>
#include <thread>
#include <chrono>

#include "CoreSystem.hpp"
#include "../Scene/Scene.h"

namespace Core
{
    class SceneManagerSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::SceneManagerSystem;

        SceneManagerSystem(std::shared_ptr<EngineContext> InContext);
        ~SceneManagerSystem() override = default;

        template <typename T>
            requires IsScene<T>
        void Push();

        void RequestPop();

        std::shared_ptr<Scene> GetActiveScene() const { return ActiveScene; }

        virtual void Start() override;
        virtual void Tick(float DeltaTimeS) override;
        virtual void Render() override;

    private:
        bool Pop();
        std::stack<std::shared_ptr<Scene>> Scenes;
        std::shared_ptr<Scene> ActiveScene{nullptr};
        bool bPopRequested = false;
    };

    template <typename T> requires IsScene<T>
    void SceneManagerSystem::Push()
    {
        auto NewScene = std::make_shared<T>(GetContext());
        Scenes.push(NewScene);
        ActiveScene = Scenes.top();

        Task<> LoadTask = NewScene->Load();
        while (!LoadTask.await_ready())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        NewScene->Enter();
    }
}
