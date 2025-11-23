#pragma once
#include <unordered_map>

#include "../Assets/AssetId.hpp"
#include "CoreSystem.hpp"

namespace sf
{
    class Texture;
    class Font;
    class SoundBuffer;
}

namespace Core
{
    class AssetRegistrySystem : public CoreSystem
    {
    public:
        constexpr static ECoreSystemType Type = ECoreSystemType::AssetRegistrySystem;

        AssetRegistrySystem(const std::shared_ptr<EngineContext>& InContext);

        template <typename T>
        AssetId Store(std::shared_ptr<T> Asset);

        template <typename T>
        std::shared_ptr<T> Get(AssetId Id);

    private:
        template <typename T>
        std::unordered_map<AssetId, std::shared_ptr<T>>* GetAssetMap();

        std::unordered_map<AssetId, std::shared_ptr<sf::Texture>> AssetToTexture;
        std::unordered_map<AssetId, std::shared_ptr<sf::Font>> AssetToFont;
        std::unordered_map<AssetId, std::shared_ptr<sf::SoundBuffer>> AssetToSound;
    };

    template <typename T>
    AssetId AssetRegistrySystem::Store(std::shared_ptr<T> Asset)
    {
        AssetId Id;
        GetAssetMap<T>()->emplace(Id, Asset);
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
}
