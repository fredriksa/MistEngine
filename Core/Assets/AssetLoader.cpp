#include "AssetLoader.h"

#include "../Systems/AssetRegistrySystem.h"
#include "../Systems/DataAssetRegistrySystem.h"
#include <future>
#include "../Async/Awaitable.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include "DataAsset.h"
#include "../Utils/StringUtils.h"


namespace Core
{
    AssetLoader::AssetLoader(std::shared_ptr<AssetRegistrySystem> AssetRegistry,
                             std::shared_ptr<DataAssetRegistrySystem> DataAssetRegistry)
        : AssetRegistry(std::move(AssetRegistry)), DataAssetRegistry(std::move(DataAssetRegistry))
    {
    }

    void AssetLoader::QueueTexture(const std::string& Path)
    {
        if (ShouldQueue(Path, AssetType::Texture))
        {
            QueuedRequests.push_back({AssetType::Texture, Path});
        }
    }

    void AssetLoader::QueueFont(const std::string& Path, unsigned int FontSize)
    {
        if (ShouldQueue(Path, AssetType::Font))
        {
            QueuedRequests.push_back({AssetType::Font, Path, FontSize});
        }
    }

    void AssetLoader::QueueSound(const std::string& Path)
    {
        if (ShouldQueue(Path, AssetType::Sound))
        {
            QueuedRequests.push_back({AssetType::Sound, Path});
        }
    }

    void AssetLoader::QueueObject(const std::string Type)
    {
        const std::string ObjectPath = "Game/Assets/Objects/" + ToLowercase(Type) + ".json";

        if (ShouldQueue(ObjectPath, AssetType::Object))
        {
            QueuedRequests.push_back({AssetType::Object, ObjectPath});
        }
    }

    void AssetLoader::Clear()
    {
        QueuedRequests.clear();
    }

    void AssetLoader::SeparateRequestsByType(
        std::vector<LoadRequest>& OutObjectRequests,
        std::vector<LoadRequest>& OutBinaryRequests)
    {
        for (const auto& Request : QueuedRequests)
        {
            if (Request.Type == AssetType::Object)
            {
                OutObjectRequests.push_back(Request);
            }
            else
            {
                OutBinaryRequests.push_back(Request);
            }
        }
    }

    Task<std::vector<AssetLoader::LoadedAsset>> AssetLoader::LoadAssetsInParallel(
        const std::vector<LoadRequest>& Requests)
    {
        std::vector<std::future<LoadedAsset>> Futures;
        Futures.reserve(Requests.size());

        for (const auto& Request : Requests)
        {
            Futures.push_back(std::async(std::launch::async, [this, Request]()
            {
                LoadedAsset Asset = LoadAsset(Request);
                ++CompletedCount;
                return Asset;
            }));
        }

        std::vector<LoadedAsset> Results;
        Results.reserve(Futures.size());
        for (auto& Future : Futures)
        {
            Results.push_back(co_await Future);
        }

        co_return Results;
    }

    void AssetLoader::ProcessLoadedDataAssets(const std::vector<LoadedAsset>& LoadedAssets)
    {
        for (const auto& Result : LoadedAssets)
        {
            if (Result.Success)
            {
                auto LoadedDataAsset = std::static_pointer_cast<DataAsset>(Result.Data);
                DataAssetRegistry->Store(LoadedDataAsset->Name, LoadedDataAsset);

                for (const auto& Component : LoadedDataAsset->Components)
                {
                    std::vector<AssetReference> DiscoveredAssets;
                    ScanForAssets(Component.Data, DiscoveredAssets);

                    for (const auto& AssetRef : DiscoveredAssets)
                    {
                        switch (AssetRef.Type)
                        {
                        case AssetType::Texture:
                            QueueTexture(AssetRef.Path);
                            break;
                        case AssetType::Sound:
                            QueueSound(AssetRef.Path);
                            break;
                        case AssetType::Font:
                            QueueFont(AssetRef.Path);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
            else
            {
                std::printf("Could not load DataAsset: %s\n", Result.Path.c_str());
            }
        }
    }

    std::vector<AssetId> AssetLoader::ProcessLoadedBinaryAssets(const std::vector<LoadedAsset>& LoadedAssets)
    {
        std::vector<AssetId> LoadedIds;

        for (const auto& Result : LoadedAssets)
        {
            if (Result.Success)
            {
                AssetId Id;
                switch (Result.Type)
                {
                case AssetType::Texture:
                    Id = AssetRegistry->Store<sf::Texture>(
                        std::static_pointer_cast<sf::Texture>(Result.Data),
                        Result.Path
                    );
                    break;
                case AssetType::Font:
                    Id = AssetRegistry->Store<sf::Font>(
                        std::static_pointer_cast<sf::Font>(Result.Data),
                        Result.Path
                    );
                    break;
                case AssetType::Sound:
                    Id = AssetRegistry->Store<sf::SoundBuffer>(
                        std::static_pointer_cast<sf::SoundBuffer>(Result.Data),
                        Result.Path
                    );
                    break;
                default:
                    assert(false);
                    break;
                }
                LoadedIds.push_back(Id);
            }
            else
            {
                std::printf("Could not load asset with path: %s\n", Result.Path.c_str());
            }
        }

        return LoadedIds;
    }

    Task<std::vector<AssetId>> AssetLoader::LoadAllAsync()
    {
        TotalCount = static_cast<int>(QueuedRequests.size());
        CompletedCount = 0;

        std::vector<LoadRequest> ObjectRequests;
        std::vector<LoadRequest> BinaryRequests;
        SeparateRequestsByType(ObjectRequests, BinaryRequests);

        std::vector<LoadedAsset> LoadedDataAssets = co_await LoadAssetsInParallel(ObjectRequests);
        ProcessLoadedDataAssets(LoadedDataAssets);

        TotalCount = static_cast<int>(QueuedRequests.size());

        BinaryRequests.clear();
        for (const auto& Request : QueuedRequests)
        {
            if (Request.Type != AssetType::Object)
            {
                BinaryRequests.push_back(Request);
            }
        }

        std::vector<LoadedAsset> LoadedBinaryAssets = co_await LoadAssetsInParallel(BinaryRequests);
        std::vector<AssetId> AllLoadedIds = ProcessLoadedBinaryAssets(LoadedBinaryAssets);

        QueuedRequests.clear();
        co_return AllLoadedIds;
    }

    float AssetLoader::GetProgress() const
    {
        if (TotalCount == 0)
        {
            return 1.f;
        }
        return static_cast<float>(CompletedCount.load()) / static_cast<float>(TotalCount.load());
    }

    bool AssetLoader::ShouldQueue(const std::string& Path, AssetType Type) const
    {
        if (AssetRegistry->Contains(Path))
        {
            return false;
        }

        // :TODO: Usage of this func can be made more performant instead of iterating through all the queued requests
        // for each asset to be loaded at least once.
        return !std::any_of(QueuedRequests.begin(), QueuedRequests.end(), [&](const LoadRequest& Request)
        {
            return Type == Request.Type && Request.Path == Path;
        });
    }

    AssetLoader::AssetType AssetLoader::GetAssetTypeFromExtension(const std::string& Path) const
    {
        if (Path.ends_with(".png") || Path.ends_with(".jpg") ||
            Path.ends_with(".jpeg") || Path.ends_with(".bmp"))
        {
            return AssetType::Texture;
        }
        if (Path.ends_with(".wav") || Path.ends_with(".ogg") || Path.ends_with(".mp3"))
        {
            return AssetType::Sound;
        }
        if (Path.ends_with(".ttf") || Path.ends_with(".otf"))
        {
            return AssetType::Font;
        }

        return AssetType::Invalid;
    }

    void AssetLoader::ScanForAssets(const nlohmann::json& Data, std::vector<AssetReference>& OutAssets)
    {
        if (Data.is_string())
        {
            std::string str = Data.get<std::string>();
            AssetType type = GetAssetTypeFromExtension(str);
            if (type != AssetType::Invalid)
            {
                OutAssets.push_back({str, type});
            }
        }
        else if (Data.is_object())
        {
            for (auto& [Key, value] : Data.items())
            {
                ScanForAssets(value, OutAssets);
            }
        }
        else if (Data.is_array())
        {
            for (auto& Element : Data)
            {
                ScanForAssets(Element, OutAssets);
            }
        }

        // Don't check primitives e.g., ints and don't recurse further
    }

    AssetLoader::LoadedAsset AssetLoader::LoadAsset(const LoadRequest& Request)
    {
        LoadedAsset Result;
        Result.Type = Request.Type;
        Result.Path = Request.Path;
        Result.Success = false;

        try
        {
            switch (Request.Type)
            {
            case AssetType::Texture:
                {
                    auto texture = std::make_shared<sf::Texture>();
                    if (texture->loadFromFile(Request.Path))
                    {
                        Result.Data = texture;
                        Result.Success = true;
                    }
                    else
                    {
                        Result.ErrorMessage = "Failed to load texture";
                    }
                }
                break;

            case AssetType::Font:
                {
                    auto font = std::make_shared<sf::Font>();
                    if (font->openFromFile(Request.Path))
                    {
                        Result.Data = font;
                        Result.Success = true;
                    }
                    else
                    {
                        Result.ErrorMessage = "Failed to load font";
                    }
                }
                break;

            case AssetType::Sound:
                {
                    auto sound = std::make_shared<sf::SoundBuffer>();
                    if (sound->loadFromFile(Request.Path))
                    {
                        Result.Data = sound;
                        Result.Success = true;
                    }
                    else
                    {
                        Result.ErrorMessage = "Failed to load sound";
                    }
                }
                break;
            case AssetType::Object:
                {
                    if (std::optional<DataAsset> LoadedDataAsset = DataAsset::LoadFromFile(Request.Path))
                    {
                        Result.Data = std::make_shared<DataAsset>(
                            std::move(LoadedDataAsset.value()));
                        Result.Success = true;
                    }
                    else
                    {
                        Result.ErrorMessage = "Failed to load asset";
                    }
                    break;
                }
            default:
                assert(false);
                break;
            }
        }
        catch (const std::exception& e)
        {
            Result.ErrorMessage = e.what();
        }

        return Result;
    }
}
