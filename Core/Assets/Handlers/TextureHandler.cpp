#include "TextureHandler.h"

#include <SFML/Graphics/Texture.hpp>

#include "../AssetLoader.h"
#include "../../Systems/AssetRegistrySystem.h"

namespace Core
{
    AssetLoader::LoadedAsset TextureHandler::Load(const AssetLoader::LoadRequest& Request)
    {
        auto Texture = std::make_shared<sf::Texture>();
        AssetLoader::LoadedAsset Result{AssetLoader::AssetType::Texture, Request.Path};
        Result.Success = Texture->loadFromFile(Request.Path);
        Result.Data = Texture;
        if (!Result.Success)
        {
            Result.ErrorMessage = "Failed to load texture";
        }
        return Result;
    }

    AssetId TextureHandler::Store(std::shared_ptr<void> Data, const std::string& Path,
                                  AssetRegistrySystem& Registry)
    {
        return Registry.Store<sf::Texture>(std::static_pointer_cast<sf::Texture>(Data), Path);
    }

    void TextureHandler::Unload(AssetId Id, AssetRegistrySystem& Registry)
    {
        Registry.Unload(Id);
    }
}
