#pragma once

#include "CoreSystem.hpp"
#include "../Coordinates/WindowCoordinate.h"
#include "../Coordinates/WorldCoordinate.h"
#include "../Coordinates/TileCoordinate.h"

namespace sf
{
    class View;
}

namespace Core
{
    class CoordinateProjectionSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::CoordinateProjectionSystem;

        CoordinateProjectionSystem(std::shared_ptr<EngineContext> InContext);

        WorldCoordinate WindowToWorld(WindowCoordinate Window, const sf::View& View) const;
        WindowCoordinate WorldToWindow(WorldCoordinate World, const sf::View& View) const;

        TileCoordinate WorldToTile(WorldCoordinate World, float TileSize = 16.0f) const;
        WorldCoordinate TileToWorld(TileCoordinate Tile, float TileSize = 16.0f) const;

        TileCoordinate WindowToTile(WindowCoordinate Window, const sf::View& View, float TileSize = 16.0f) const;
    };
}
