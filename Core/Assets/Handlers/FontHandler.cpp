#include "FontHandler.h"

#include <SFML/Graphics/Font.hpp>

#include "../AssetLoader.h"
#include "../../Systems/AssetRegistrySystem.h"

namespace Core
{
    AssetLoader::LoadedAsset FontHandler::Load(const AssetLoader::LoadRequest& Request)
    {
        auto Font = std::make_shared<sf::Font>();
        AssetLoader::LoadedAsset Result{AssetLoader::AssetType::Font, Request.Path};
        Result.Success = Font->openFromFile(Request.Path);
        Result.Data = Font;
        if (!Result.Success)
        {
            Result.ErrorMessage = "Failed to load font";
        }
        return Result;
    }

    AssetId FontHandler::Store(std::shared_ptr<void> Data, const std::string& Path,
                               AssetRegistrySystem& Registry)
    {
        return Registry.Store<sf::Font>(std::static_pointer_cast<sf::Font>(Data), Path);
    }

    void FontHandler::Unload(AssetId Id, AssetRegistrySystem& Registry)
    {
        Registry.Unload(Id);
    }
}
