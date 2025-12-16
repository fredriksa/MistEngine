#pragma once

#include <vector>
#include <string>
#include <memory>

#include "../EngineContext.hpp"
#include "../World/World.h"
#include "../Interfaces/IRenderable.hpp"
#include "../Interfaces/ITickable.hpp"
#include "../Assets/AssetId.hpp"
#include "../Async/Task.hpp"

namespace Core
{
    class Scene : public ITickable, public IRenderable
    {
    public:
        Scene(std::shared_ptr<EngineContext> Context, std::string Name);
        virtual ~Scene();
        const std::string& GetName() { return Name; }

        void Tick(float DeltaTimeS) final;
        void Render() final;

        Task<> Load();
        void Enter();
        void Exit();

    protected:
        virtual void OnLoad()
        {
        }

        virtual void OnEnter()
        {
        }

        virtual void OnExit()
        {
        }

        virtual void PreRender()
        {
        }

        virtual void PostRender()
        {
        }

    protected:
        std::shared_ptr<EngineContext> Context;
        std::vector<AssetId> LoadedAssets;
        std::string Name;
        World World;
    };

    template <typename T>
    concept IsScene = std::is_base_of_v<Scene, T>;
}
