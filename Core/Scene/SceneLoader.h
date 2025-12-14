#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Async/Task.hpp"
#include "../Assets/AssetId.hpp"

namespace Core
{
    struct EngineContext;
    struct AssetManifest;
    class World;

    class SceneLoader
    {
    public:
        SceneLoader(std::shared_ptr<EngineContext> Context);
        ~SceneLoader();

        Task<std::vector<AssetId>> LoadScene(const std::string& SceneName, World& TargetWorld);

    private:
        void QueueAssetsFromManifest(const AssetManifest& Manifest);
        void SpawnObjectsIntoWorld(const AssetManifest& Manifest, World& TargetWorld);

        std::shared_ptr<EngineContext> Context;
        std::unique_ptr<class AssetLoader> Loader;
    };
}
