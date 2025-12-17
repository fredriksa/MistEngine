#include "TileSheet.h"

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
        return TileIndex / Rows;
    }

    uint TileSheet::GetTileCount() const
    {
        return Rows * Columns;
    }

    std::optional<TileSheet> TileSheet::LoadFromFile(const std::string& AbsolutePath)
    {
        TileSheet Sheet;
        Sheet.Name = std::filesystem::path(AbsolutePath).stem().string();
        Sheet.AbsolutePath = AbsolutePath;

        Sheet.Texture = std::make_shared<sf::Texture>();
        if (!Sheet.Texture->loadFromFile(AbsolutePath))
        {
            return std::nullopt;
        }

        sf::Vector2u Size = Sheet.Texture->getSize();
        Sheet.Columns = Size.x / Sheet.Texture->getSize().x;
        Sheet.Rows = Size.y / Sheet.Texture->getSize().y;
        return Sheet;
    }
}
