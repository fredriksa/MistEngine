#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Async/Task.hpp"
#include "AssetId.hpp"
#include "../ThirdParty/json.hpp"

namespace Core
{
    class AssetRegistrySystem;
    class DataAssetRegistrySystem;
}

namespace Core
{
    class AssetLoader
    {
    private:
        enum class AssetType
        {
            Invalid,
            Texture,
            Font,
            Sound,
            Object
        };

    public:
        AssetLoader(std::shared_ptr<AssetRegistrySystem> AssetRegistry,
                    std::shared_ptr<DataAssetRegistrySystem> DataAssetRegistry);

        void QueueTexture(const std::string& Path);
        void QueueFont(const std::string& Path, unsigned int FontSize = 16);
        void QueueSound(const std::string& Path);
        void QueueObject(std::string Type);

        void Clear();

        Task<std::vector<AssetId>> LoadAllAsync();

        float GetProgress() const;
        int GetCompletedCount() const { return CompletedCount; }

        bool ShouldQueue(const std::string& Path, AssetType Type) const;

    private:
        struct AssetReference
        {
            std::string Path;
            AssetType Type;
        };

        AssetType GetAssetTypeFromExtension(const std::string& Path) const;
        void ScanForAssets(const nlohmann::json& Data, std::vector<AssetReference>& OutAssets);

    private:
        std::shared_ptr<AssetRegistrySystem> AssetRegistry;
        std::shared_ptr<DataAssetRegistrySystem> DataAssetRegistry;

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
