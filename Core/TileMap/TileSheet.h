#pragma once

#include <optional>
#include <memory>
#include <string>
#include <SFML/Graphics/Texture.hpp>

#include "../Common.h"

namespace Core
{
    class TileSheet
    {
    public:
        static std::optional<TileSheet> LoadFromFile(const std::string& AbsolutePath);

        uint GetId() const { return Id; }
        const std::string& GetName() const { return Name; }
        const std::string& GetAbsolutePath() const { return AbsolutePath; }
        const std::shared_ptr<const sf::Texture>& GetTexture() const { return Texture; }
        uint GetNumColumns() const { return Columns; }
        uint GetNumRows() const { return Rows; }
        const sf::Vector2u& GetTileDimensions() const { return TileDimensions; }

        sf::IntRect GetTileRect(uint TileIndex) const;
        uint GetTileColumn(uint TileIndex) const;
        uint GetTileRow(uint TileIndex) const;
        uint GetTileCount() const;

    private:
        uint Id = 0;
        std::string Name;
        std::string AbsolutePath;
        std::shared_ptr<sf::Texture> Texture;
        uint Columns = 0;
        uint Rows = 0;
        sf::Vector2u TileDimensions = sf::Vector2u(16, 16);
    };
}
