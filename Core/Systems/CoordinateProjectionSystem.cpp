#include "CoordinateProjectionSystem.h"
#include "../EngineContext.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <cmath>

namespace Core
{
    CoordinateProjectionSystem::CoordinateProjectionSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("CoordinateProjectionSystem", Type, std::move(InContext))
    {
    }

    WorldCoordinate CoordinateProjectionSystem::WindowToWorld(WindowCoordinate Window, const sf::View& View) const
    {
        sf::Vector2f WorldCoords = GetContext()->Window->mapPixelToCoords(Window.Value, View);
        return WorldCoordinate(WorldCoords);
    }

    WindowCoordinate CoordinateProjectionSystem::WorldToWindow(WorldCoordinate World, const sf::View& View) const
    {
        sf::Vector2i WindowCoords = GetContext()->Window->mapCoordsToPixel(World.Value, View);
        return WindowCoordinate(WindowCoords);
    }

    TileCoordinate CoordinateProjectionSystem::WorldToTile(WorldCoordinate World, float TileSize) const
    {
        int TileX = static_cast<int>(std::floor(World.X() / TileSize));
        int TileY = static_cast<int>(std::floor(World.Y() / TileSize));
        return TileCoordinate(TileX, TileY);
    }

    WorldCoordinate CoordinateProjectionSystem::TileToWorld(TileCoordinate Tile, float TileSize) const
    {
        float WorldX = Tile.X() * TileSize + TileSize * 0.5f;
        float WorldY = Tile.Y() * TileSize + TileSize * 0.5f;
        return WorldCoordinate(WorldX, WorldY);
    }

    TileCoordinate CoordinateProjectionSystem::WindowToTile(WindowCoordinate Window, const sf::View& View,
                                                            float TileSize) const
    {
        WorldCoordinate World = WindowToWorld(Window, View);
        return WorldToTile(World, TileSize);
    }
}
