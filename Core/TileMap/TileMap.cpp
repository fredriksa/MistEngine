#include "TileMap.h"

namespace Core
{
    TileMap::TileMap(uint Width, uint Height)
        : Width(Width)
          , Height(Height)
    {
        AddLayer();
    }

    void TileMap::SetTile(uint X, uint Y, uint Layer, uint TileSheetId, uint TileIndex)
    {
        if (!IsValidCoordinate(X, Y) || !IsValidLayer(Layer))
        {
            return;
        }

        Layers[Layer][GetIndex(X, Y)] = Tile(TileSheetId, TileIndex);
    }

    void TileMap::SetTile(uint X, uint Y, uint Layer, const Tile& InTile)
    {
        if (!IsValidCoordinate(X, Y) || !IsValidLayer(Layer))
        {
            return;
        }

        Layers[Layer][GetIndex(X, Y)] = InTile;
    }

    const Tile& TileMap::GetTile(uint X, uint Y, uint Layer) const
    {
        static const Tile EmptyTile;

        if (!IsValidCoordinate(X, Y) || !IsValidLayer(Layer))
        {
            return EmptyTile;
        }

        return Layers[Layer][GetIndex(X, Y)];
    }

    Tile& TileMap::GetTile(uint X, uint Y, uint Layer)
    {
        static Tile EmptyTile;

        if (!IsValidCoordinate(X, Y) || !IsValidLayer(Layer))
        {
            return EmptyTile;
        }

        return Layers[Layer][GetIndex(X, Y)];
    }

    void TileMap::Clear()
    {
        for (auto& Layer : Layers)
        {
            for (Tile& tile : Layer)
            {
                tile = Tile();
            }
        }
    }

    void TileMap::ClearLayer(uint Layer)
    {
        if (!IsValidLayer(Layer))
        {
            return;
        }

        for (Tile& tile : Layers[Layer])
        {
            tile = Tile();
        }
    }

    void TileMap::AddLayer()
    {
        std::vector<Tile> NewLayer;
        NewLayer.resize(Width * Height, Tile());
        Layers.push_back(std::move(NewLayer));
    }

    void TileMap::RemoveLayer(uint Layer)
    {
        if (!IsValidLayer(Layer) || Layers.size() <= 1)
        {
            return;
        }

        Layers.erase(Layers.begin() + Layer);
    }

    void TileMap::SwapLayers(uint LayerA, uint LayerB)
    {
        if (!IsValidLayer(LayerA) || !IsValidLayer(LayerB))
        {
            return;
        }

        if (LayerA == LayerB)
        {
            return;
        }

        std::swap(Layers[LayerA], Layers[LayerB]);
    }

    const std::vector<Tile>& TileMap::GetLayerTiles(uint Layer) const
    {
        static const std::vector<Tile> EmptyLayer;

        if (!IsValidLayer(Layer))
        {
            return EmptyLayer;
        }

        return Layers[Layer];
    }

    bool TileMap::IsValidCoordinate(uint X, uint Y) const
    {
        return X < Width && Y < Height;
    }

    bool TileMap::IsValidLayer(uint Layer) const
    {
        return Layer < Layers.size();
    }

    uint TileMap::GetIndex(uint X, uint Y) const
    {
        return Y * Width + X;
    }
}
