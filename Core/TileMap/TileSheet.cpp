#include "TileSheet.h"
#include "../Systems/AssetRegistrySystem.h"

namespace Core
{
    sf::IntRect TileSheet::GetTileRect(uint TileIndex) const
    {
        if (Columns == 0 || Rows == 0)
        {
            return {};
        }

        const uint Column = GetTileColumn(TileIndex);
        const uint Row = GetTileRow(TileIndex);

        return {
            sf::Vector2i(static_cast<int>(Column * TileDimensions.x),
                         static_cast<int>(Row * TileDimensions.y)),
            sf::Vector2i(static_cast<int>(TileDimensions.x), static_cast<int>(TileDimensions.y))
        };
    }

    uint TileSheet::GetTileColumn(uint TileIndex) const
    {
        return TileIndex % Columns;
    }

    uint TileSheet::GetTileRow(uint TileIndex) const
    {
        return TileIndex / Columns;
    }

    uint TileSheet::GetTileCount() const
    {
        return Rows * Columns;
    }

    std::optional<TileSheet> TileSheet::Create(const std::string& Path, AssetRegistrySystem* Registry)
    {
        std::shared_ptr<const sf::Texture> Texture = Registry->Get<sf::Texture>(Path);
        if (!Texture)
        {
            return std::nullopt;
        }

        TileSheet Sheet;
        Sheet.Name = std::filesystem::path(Path).stem().string();
        Sheet.AbsolutePath = Path;
        Sheet.Texture = std::const_pointer_cast<sf::Texture>(Texture);

        sf::Vector2u Size = Sheet.Texture->getSize();
        Sheet.Columns = Size.x / Sheet.TileDimensions.x;
        Sheet.Rows = Size.y / Sheet.TileDimensions.y;

        return Sheet;
    }
}
