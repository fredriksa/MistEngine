#pragma once
#include <stack>

#include "CoreSystem.hpp"
#include "../Scene.h"

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

        bool Pop();

        virtual void Start() override;
        virtual void Tick(float DeltaTimeS) override;
        virtual void Render() override;

    private:
        std::stack<std::shared_ptr<Scene>> Scenes;
        std::shared_ptr<Scene> ActiveScene{nullptr};
    };

    template <typename T> requires IsScene<T>
    void SceneManagerSystem::Push()
    {
        Scenes.push(std::make_shared<T>(GetContext()));
        ActiveScene = Scenes.top();
    }
}
