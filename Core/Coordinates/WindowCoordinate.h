#pragma once

#include <SFML/System/Vector2.hpp>

namespace Core
{
    struct WindowCoordinate
    {
        sf::Vector2i Value;

        WindowCoordinate() : Value(0, 0)
        {
        }

        WindowCoordinate(int X, int Y) : Value(X, Y)
        {
        }

        explicit WindowCoordinate(sf::Vector2i V) : Value(V)
        {
        }

        int X() const { return Value.x; }
        int Y() const { return Value.y; }

        operator sf::Vector2i() const { return Value; }

        bool operator==(const WindowCoordinate& Other) const { return Value == Other.Value; }
        bool operator!=(const WindowCoordinate& Other) const { return Value != Other.Value; }
    };
}
