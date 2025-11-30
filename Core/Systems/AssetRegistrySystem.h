#pragma once
#include <unordered_map>
#include <string>

#include "../Assets/AssetId.hpp"
#include "../Assets/AssetMetadata.h"
#include "CoreSystem.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

namespace Core
{
    class AssetRegistrySystem : public CoreSystem
    {
    public:
        constexpr static ECoreSystemType Type = ECoreSystemType::AssetRegistrySystem;

        AssetRegistrySystem(const std::shared_ptr<EngineContext>& InContext);

        void Shutdown() override;

        template <typename T>
        AssetId Store(std::shared_ptr<T> Asset, const std::string& Path);

        void Release(AssetId Id);

        template <typename T>
        std::shared_ptr<T> Get(AssetId Id);

    private:
        void Unload(AssetId Id);

        template <typename T>
        std::unordered_map<AssetId, std::shared_ptr<T>>* GetAssetMap();

        template <typename T>
        AssetType GetAssetType();

        std::unordered_map<AssetId, std::shared_ptr<sf::Texture>> AssetToTexture;
        std::unordered_map<AssetId, std::shared_ptr<sf::Font>> AssetToFont;
        std::unordered_map<AssetId, std::shared_ptr<sf::SoundBuffer>> AssetToSound;

        std::unordered_map<AssetId, AssetMetadata> Metadata;
        std::unordered_map<AssetId, int> RefCounts;

        std::unordered_map<std::string, AssetId> PathToAssetId;
    };

    template <typename T>
    AssetId AssetRegistrySystem::Store(std::shared_ptr<T> Asset, const std::string& Path)
    {
        if (PathToAssetId.contains(Path))
        {
            AssetId ExistingId = PathToAssetId[Path];
            RefCounts[ExistingId]++;
            return ExistingId;
        }

        AssetId Id;
        GetAssetMap<T>()->emplace(Id, Asset);

        PathToAssetId[Path] = Id;
        RefCounts[Id] = 1;
        Metadata[Id] = {.Type = GetAssetType<T>(), .Path = Path};

        return Id;
    }

    template <typename T>
    std::shared_ptr<T> AssetRegistrySystem::Get(AssetId AssetId)
    {
        auto AssetMap = GetAssetMap<T>();
        auto it = AssetMap->find(AssetId);
        if (it != AssetMap->end())
        {
            return it->second;
        }

        return nullptr;
    }

    template <typename T>
    std::unordered_map<AssetId, std::shared_ptr<T>>* AssetRegistrySystem::GetAssetMap()
    {
        if constexpr (std::is_same_v<T, sf::Texture>)
        {
            return &AssetToTexture;
        }
        else if constexpr (std::is_same_v<T, sf::Font>)
        {
            return &AssetToFont;
        }
        else if constexpr (std::is_same_v<T, sf::SoundBuffer>)
        {
            return &AssetToSound;
        }

        return nullptr;
    }

    template <typename T>
    AssetType AssetRegistrySystem::GetAssetType()
    {
        if constexpr (std::is_same_v<T, sf::Texture>)
        {
            return AssetType::Texture;
        }
        else if constexpr (std::is_same_v<T, sf::Font>)
        {
            return AssetType::Font;
        }
        else if constexpr (std::is_same_v<T, sf::SoundBuffer>)
        {
            return AssetType::Sound;
        }

        return AssetType::INVALID;
    }
}
