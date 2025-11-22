#pragma once

#include <SFML/System/Vector2.hpp>

namespace Core
{
    class Transform
    {
    public:
        void SetAngle(double InAngle) { Angle = InAngle; }
        double GetAngle() const { return Angle; }

        sf::Vector2f& GetPosition() { return Position; }

    private:
        sf::Vector2f Position = sf::Vector2f(0, 0);
        double Angle = 0;
    };
}
