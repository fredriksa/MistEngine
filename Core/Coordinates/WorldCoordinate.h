#pragma once

#include <SFML/System/Vector2.hpp>

namespace Core
{
    struct WorldCoordinate
    {
        sf::Vector2f Value;

        WorldCoordinate() : Value(0.0f, 0.0f)
        {
        }

        WorldCoordinate(float X, float Y) : Value(X, Y)
        {
        }

        explicit WorldCoordinate(sf::Vector2f V) : Value(V)
        {
        }

        float X() const { return Value.x; }
        float Y() const { return Value.y; }

        operator sf::Vector2f() const { return Value; }

        WorldCoordinate operator+(const WorldCoordinate& Other) const { return WorldCoordinate(Value + Other.Value); }
        WorldCoordinate operator-(const WorldCoordinate& Other) const { return WorldCoordinate(Value - Other.Value); }
        WorldCoordinate operator*(float Scalar) const { return WorldCoordinate(Value * Scalar); }
        WorldCoordinate operator/(float Scalar) const { return WorldCoordinate(Value / Scalar); }

        bool operator==(const WorldCoordinate& Other) const { return Value == Other.Value; }
        bool operator!=(const WorldCoordinate& Other) const { return Value != Other.Value; }
    };
}
