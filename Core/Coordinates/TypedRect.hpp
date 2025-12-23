#pragma once
#include "ScreenCoordinate.h"
#include "WindowCoordinate.h"
#include "WorldCoordinate.h"
#include "TileCoordinate.h"

namespace Core
{
    template <typename CoordType>
    struct TypedRect
    {
        CoordType Position;
        CoordType Size;

        TypedRect() = default;

        TypedRect(const CoordType& Position, const CoordType& Size)
            : Position(Position), Size(Size)
        {
        }

        TypedRect(float X, float Y, float Width, float Height)
            : Position(X, Y), Size(Width, Height)
        {
        }

        bool Contains(const CoordType& Point) const
        {
            return Point.X() >= Position.X() &&
                Point.X() < Position.X() + Size.X() &&
                Point.Y() >= Position.Y() &&
                Point.Y() < Position.Y() + Size.Y();
        }

        float Left() const { return Position.X(); }
        float Top() const { return Position.Y(); }
        float Right() const { return Position.X() + Size.X(); }
        float Bottom() const { return Position.Y() + Size.Y(); }
        float Width() const { return Size.X(); }
        float Height() const { return Size.Y(); }

        bool HasArea() const { return Width() > 0 && Height() > 0; }
    };

    using ScreenRect = TypedRect<ScreenCoordinate>;
    using WorldRect = TypedRect<WorldCoordinate>;
    using WindowRect = TypedRect<WindowCoordinate>;
    using TileRectCoord = TypedRect<TileCoordinate>;
}
