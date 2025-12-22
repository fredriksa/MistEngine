#include "Tile.h"

namespace Core
{
	nlohmann::json Tile::ToJson() const
	{
		if (TileSheetId.has_value())
		{
			return {
				{"tileSheetId", TileSheetId.value()},
				{"tileIndex", TileIndex}
			};
		}
		else
		{
			return {
				{"tileSheetId", nullptr},
				{"tileIndex", TileIndex}
			};
		}
	}

	Tile Tile::FromJson(const nlohmann::json& Json)
	{
		if (Json["tileSheetId"].is_null())
		{
			return Tile();
		}
		else
		{
			return Tile(
				Json["tileSheetId"].get<uint>(),
				Json["tileIndex"].get<uint>()
			);
		}
	}
}
