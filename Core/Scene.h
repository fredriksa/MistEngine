#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "EngineContext.hpp"
#include "World/World.h"
#include "Interfaces/IRenderable.hpp"
#include "Interfaces/ITickable.hpp"
#include "Assets/AssetId.hpp"
#include "Async/Task.hpp"

namespace Core
{
    class AssetRegistrySystem;
    class AssetLoader;

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

    protected:
        std::vector<AssetId> LoadedAssets;
        std::string Name;
        std::shared_ptr<EngineContext> Context;
        World World;

    private:
        std::unique_ptr<AssetLoader> Loader;
    };

    template <typename T>
    concept IsScene = std::is_base_of_v<Scene, T>;
}
