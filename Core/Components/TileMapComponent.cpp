#include "TileMapComponent.h"

#include "ComponentRegistry.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/AssetRegistrySystem.h"
#include "../TileMap/TileSheet.h"
#include "../World/WorldConstants.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace Core
{
    REGISTER_COMPONENT(TileMapComponent);

    TileMapComponent::TileMapComponent(const std::shared_ptr<WorldObject>& Owner)
        : Component(Owner)
    {
    }

    bool TileMapComponent::Initialize(const nlohmann::json& Data)
    {
        uint Width = Data.value("width", 0u);
        uint Height = Data.value("height", 0u);

        if (Width == 0 || Height == 0)
        {
            return false;
        }

        TileMapData = TileMap(Width, Height);

        if (Data.contains("tiles") && Data["tiles"].is_array())
        {
            const nlohmann::json& TilesArray = Data["tiles"];
            for (size_t i = 0; i < TilesArray.size() && i < TileMapData.GetTiles().size(); ++i)
            {
                Tile LoadedTile = Tile::FromJson(TilesArray[i]);
                uint X = i % Width;
                uint Y = i / Width;
                TileMapData.SetTile(X, Y, LoadedTile);
            }
        }

        return true;
    }

    void TileMapComponent::Render()
    {
        const EngineContext& Context = GetContext();
        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context.SystemsRegistry->GetCoreSystem<
            AssetRegistrySystem>();
        if (!AssetRegistry)
        {
            return;
        }

        std::vector<std::shared_ptr<const TileSheet>> TileSheets = AssetRegistry->GetAllTileSheets();
        if (TileSheets.empty())
        {
            return;
        }

        for (uint Y = 0; Y < TileMapData.GetHeight(); ++Y)
        {
            for (uint X = 0; X < TileMapData.GetWidth(); ++X)
            {
                const Tile& Tile = TileMapData.GetTile(X, Y);

                if (Tile.IsEmpty())
                {
                    continue;
                }

                std::optional<uint> TileSheetId = Tile.GetTileSheetId();
                uint TileSheetIndex = TileSheetId.value();

                if (TileSheetIndex >= TileSheets.size())
                {
                    continue;
                }

                std::shared_ptr<const TileSheet> TileSheet = TileSheets[TileSheetIndex];
                if (!TileSheet)
                {
                    continue;
                }

                std::shared_ptr<const sf::Texture> Texture = TileSheet->GetTexture();
                if (!Texture)
                {
                    continue;
                }

                sf::IntRect TileRect = TileSheet->GetTileRect(Tile.GetTileIndex());

                sf::Sprite TileSprite(*Texture);
                TileSprite.setTextureRect(TileRect);
                TileSprite.setPosition(sf::Vector2f(X * WorldConstants::TileSize, Y * WorldConstants::TileSize));

                Context.Window->draw(TileSprite);
            }
        }
    }

    nlohmann::json TileMapComponent::ToJson() const
    {
        nlohmann::json TilesArray = nlohmann::json::array();
        for (const Tile& Tile : TileMapData.GetTiles())
        {
            TilesArray.push_back(Tile.ToJson());
        }

        return {
            {"width", TileMapData.GetWidth()},
            {"height", TileMapData.GetHeight()},
            {"tiles", TilesArray}
        };
    }
}
