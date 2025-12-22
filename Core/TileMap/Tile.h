#pragma once

#include "../ThirdParty/json.hpp"
#include "../Common.h"
#include <optional>

namespace Core
{
    class Tile
    {
    public:
        Tile() = default;

        Tile(uint TileSheetId, uint TileIndex)
            : TileSheetId(TileSheetId), TileIndex(TileIndex)
        {
        }

        std::optional<uint> GetTileSheetId() const { return TileSheetId; }
        uint GetTileIndex() const { return TileIndex; }
        bool IsEmpty() const { return !TileSheetId.has_value(); }

        nlohmann::json ToJson() const;
        static Tile FromJson(const nlohmann::json& Json);

    private:
        std::optional<uint> TileSheetId;
        uint TileIndex = 0;
    };
}
