#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Async/Task.hpp"

namespace Core
{
    class AssetRegistrySystem;

    class AssetLoader
    {
    public:
        AssetLoader(std::shared_ptr<AssetRegistrySystem> Registry);

        void QueueTexture(const std::string& Path);
        void QueueFont(const std::string& Path, unsigned int FontSize = 16);
        void QueueSound(const std::string& Path);

        void Clear();

        Task LoadAllAsync();

        // Between 0.0 - 1.0
        float GetProgress() const;
        int GetCompletedCount() const { return CompletedCount; }

    private:
        std::shared_ptr<AssetRegistrySystem> AssetRegistry;

        enum class AssetType
        {
            Texture,
            Font,
            Sound
        };

        struct LoadRequest
        {
            AssetType Type;
            std::string Path;
            unsigned int FontSize = 16;
        };

        struct LoadedAsset
        {
            AssetType Type;
            std::string Path;
            std::shared_ptr<void> Data;
            bool Success;
            std::string ErrorMessage;
        };

        std::vector<LoadRequest> QueuedRequests;

        std::atomic<int> CompletedCount{0};
        std::atomic<int> TotalCount{0};

        LoadedAsset LoadAsset(const LoadRequest& Request);
    };
}
