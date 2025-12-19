#pragma once

#include "../ThirdParty/json.hpp"
#include "../Common.h"

namespace Core
{
    class Tile
    {
    public:
        Tile(uint TileSheetId, uint TileIndex)
            : TileSheetId(TileSheetId), TileIndex(TileIndex)
        {
        }

        uint GetTileSheetId() const { return TileSheetId; }
        uint GetTileIndex() const { return TileIndex; }
        bool IsEmpty() const { return TileSheetId == 0; }

        nlohmann::json ToJson() const;
        static Tile FromJson(const nlohmann::json& Json);

    private:
        uint TileSheetId;
        uint TileIndex;
    };
}
