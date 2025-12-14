#include "SoundHandler.h"

#include <SFML/Audio/SoundBuffer.hpp>

#include "../AssetLoader.h"
#include "../../Systems/AssetRegistrySystem.h"

namespace Core
{
    AssetLoader::LoadedAsset SoundHandler::Load(const AssetLoader::LoadRequest& Request)
    {
        auto SoundBuffer = std::make_shared<sf::SoundBuffer>();
        AssetLoader::LoadedAsset Result{AssetLoader::AssetType::Sound, Request.Path};
        Result.Success = SoundBuffer->loadFromFile(Request.Path);
        Result.Data = SoundBuffer;
        if (!Result.Success)
        {
            Result.ErrorMessage = "Failed to load sound";
        }
        return Result;
    }

    AssetId SoundHandler::Store(std::shared_ptr<void> Data, const std::string& Path,
                                AssetRegistrySystem& Registry)
    {
        return Registry.Store<sf::SoundBuffer>(std::static_pointer_cast<sf::SoundBuffer>(Data), Path);
    }

    void SoundHandler::Unload(AssetId Id, AssetRegistrySystem& Registry)
    {
        Registry.Unload(Id);
    }
}
