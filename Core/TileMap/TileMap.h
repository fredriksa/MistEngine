#pragma once

#include <vector>
#include "Tile.h"
#include "../Common.h"

namespace Core
{
	class TileMap
	{
	public:
		TileMap(uint Width, uint Height);

		void SetTile(uint X, uint Y, uint Layer, uint TileSheetId, uint TileIndex);
		void SetTile(uint X, uint Y, uint Layer, const Tile& InTile);
		const Tile& GetTile(uint X, uint Y, uint Layer) const;
		Tile& GetTile(uint X, uint Y, uint Layer);

		void Clear();
		void ClearLayer(uint Layer);

		void AddLayer();
		void RemoveLayer(uint Layer);
		void SwapLayers(uint LayerA, uint LayerB);
		uint GetLayerCount() const { return static_cast<uint>(Layers.size()); }

		uint GetWidth() const { return Width; }
		uint GetHeight() const { return Height; }
		const std::vector<Tile>& GetLayerTiles(uint Layer) const;

		bool IsValidCoordinate(uint X, uint Y) const;
		bool IsValidLayer(uint Layer) const;

	private:
		uint Width;
		uint Height;
		std::vector<std::vector<Tile>> Layers;

		uint GetIndex(uint X, uint Y) const;
	};
}
