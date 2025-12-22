#pragma once

#include <SFML/System/Vector2.hpp>

namespace Core
{
    struct TileCoordinate
    {
        sf::Vector2i Value;

        TileCoordinate() : Value(0, 0)
        {
        }

        TileCoordinate(int X, int Y) : Value(X, Y)
        {
        }

        explicit TileCoordinate(sf::Vector2i V) : Value(V)
        {
        }

        int X() const { return Value.x; }
        int Y() const { return Value.y; }

        operator sf::Vector2i() const { return Value; }

        bool IsValid() const { return Value.x >= 0 && Value.y >= 0; }

        bool IsWithinBounds(int Width, int Height) const
        {
            return Value.x >= 0 && Value.x < Width && Value.y >= 0 && Value.y < Height;
        }

        TileCoordinate operator+(const TileCoordinate& Other) const { return TileCoordinate(Value + Other.Value); }
        TileCoordinate operator-(const TileCoordinate& Other) const { return TileCoordinate(Value - Other.Value); }

        bool operator==(const TileCoordinate& Other) const { return Value == Other.Value; }
        bool operator!=(const TileCoordinate& Other) const { return Value != Other.Value; }
    };
}
