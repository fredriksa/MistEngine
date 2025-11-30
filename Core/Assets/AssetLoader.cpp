#include "AssetLoader.h"

#include "../Systems/AssetRegistrySystem.h"
#include <future>
#include "../Async/Awaitable.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>


namespace Core
{
    AssetLoader::AssetLoader(std::shared_ptr<AssetRegistrySystem> Registry)
        : AssetRegistry(std::move(Registry))
    {
    }

    void AssetLoader::QueueTexture(const std::string& Path)
    {
        QueuedRequests.push_back({AssetType::Texture, Path});
    }

    void AssetLoader::QueueFont(const std::string& Path, unsigned int FontSize)
    {
        QueuedRequests.push_back({AssetType::Font, Path, FontSize});
    }

    void AssetLoader::QueueSound(const std::string& Path)
    {
        QueuedRequests.push_back({AssetType::Sound, Path});
    }

    void AssetLoader::Clear()
    {
        QueuedRequests.clear();
    }

    Task<std::vector<AssetId>> AssetLoader::LoadAllAsync()
    {
        TotalCount = static_cast<int>(QueuedRequests.size());
        CompletedCount = 0;

        std::vector<std::future<LoadedAsset>> Futures;

        for (const LoadRequest& Request : QueuedRequests)
        {
            Futures.push_back(std::async(std::launch::async, [this, Request]()
            {
                LoadedAsset LoadedAsset = LoadAsset(Request);
                ++CompletedCount;
                return LoadedAsset;
            }));
        }

        std::vector<LoadedAsset> Results;
        Results.reserve(Futures.size());
        for (auto& Future : Futures)
        {
            Results.push_back(co_await Future);
        }

        std::vector<AssetId> LoadedIds;
        for (const LoadedAsset& Result : Results)
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
                }
                LoadedIds.push_back(Id);
            }
            else
            {
                std::printf("Could not load asset with path: %s\n", Result.Path.c_str());
            }
        }

        QueuedRequests.clear();
        co_return LoadedIds;
    }

    float AssetLoader::GetProgress() const
    {
        if (TotalCount == 0)
        {
            return 1.f;
        }
        return static_cast<float>(CompletedCount.load()) / static_cast<float>(TotalCount.load());
    }

    AssetLoader::LoadedAsset AssetLoader::LoadAsset(const LoadRequest& Request)
    {
        LoadedAsset result;
        result.Type = Request.Type;
        result.Path = Request.Path;
        result.Success = false;

        try
        {
            switch (Request.Type)
            {
            case AssetType::Texture:
                {
                    auto texture = std::make_shared<sf::Texture>();
                    if (texture->loadFromFile(Request.Path))
                    {
                        result.Data = texture;
                        result.Success = true;
                    }
                    else
                    {
                        result.ErrorMessage = "Failed to load texture";
                    }
                }
                break;

            case AssetType::Font:
                {
                    auto font = std::make_shared<sf::Font>();
                    if (font->openFromFile(Request.Path))
                    {
                        result.Data = font;
                        result.Success = true;
                    }
                    else
                    {
                        result.ErrorMessage = "Failed to load font";
                    }
                }
                break;

            case AssetType::Sound:
                {
                    auto sound = std::make_shared<sf::SoundBuffer>();
                    if (sound->loadFromFile(Request.Path))
                    {
                        result.Data = sound;
                        result.Success = true;
                    }
                    else
                    {
                        result.ErrorMessage = "Failed to load sound";
                    }
                }
                break;
            }
        }
        catch (const std::exception& e)
        {
            result.ErrorMessage = e.what();
        }

        return result;
    }
}
