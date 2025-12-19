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

		void SetTile(uint X, uint Y, uint TileSheetId, uint TileIndex);
		void SetTile(uint X, uint Y, const Tile& InTile);
		const Tile& GetTile(uint X, uint Y) const;
		Tile& GetTile(uint X, uint Y);

		void Clear();

		uint GetWidth() const { return Width; }
		uint GetHeight() const { return Height; }
		const std::vector<Tile>& GetTiles() const { return Tiles; }

		bool IsValidCoordinate(uint X, uint Y) const;

	private:
		uint Width;
		uint Height;
		std::vector<Tile> Tiles;

		uint GetIndex(uint X, uint Y) const;
	};
}
