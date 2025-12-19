#include "Tile.h"

namespace Core
{
	nlohmann::json Tile::ToJson() const
	{
		return {
			{"tileSheetId", TileSheetId},
			{"tileIndex", TileIndex}
		};
	}

	Tile Tile::FromJson(const nlohmann::json& Json)
	{
		return Tile(
			Json["tileSheetId"].get<uint>(),
			Json["tileIndex"].get<uint>()
		);
	}
}
