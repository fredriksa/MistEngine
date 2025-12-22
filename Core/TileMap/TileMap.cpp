#include "TileMap.h"

namespace Core
{
    TileMap::TileMap(uint Width, uint Height)
        : Width(Width)
          , Height(Height)
    {
        Tiles.resize(Width * Height, Tile());
    }

    void TileMap::SetTile(uint X, uint Y, uint TileSheetId, uint TileIndex)
    {
        if (!IsValidCoordinate(X, Y))
        {
            return;
        }

        Tiles[GetIndex(X, Y)] = Tile(TileSheetId, TileIndex);
    }

    void TileMap::SetTile(uint X, uint Y, const Tile& InTile)
    {
        if (!IsValidCoordinate(X, Y))
        {
            return;
        }

        Tiles[GetIndex(X, Y)] = InTile;
    }

    const Tile& TileMap::GetTile(uint X, uint Y) const
    {
        static const Tile EmptyTile;

        if (!IsValidCoordinate(X, Y))
        {
            return EmptyTile;
        }

        return Tiles[GetIndex(X, Y)];
    }

    Tile& TileMap::GetTile(uint X, uint Y)
    {
        static Tile EmptyTile;

        if (!IsValidCoordinate(X, Y))
        {
            return EmptyTile;
        }

        return Tiles[GetIndex(X, Y)];
    }

    void TileMap::Clear()
    {
        for (Tile& tile : Tiles)
        {
            tile = Tile();
        }
    }

    bool TileMap::IsValidCoordinate(uint X, uint Y) const
    {
        return X < Width && Y < Height;
    }

    uint TileMap::GetIndex(uint X, uint Y) const
    {
        return Y * Width + X;
    }
}
