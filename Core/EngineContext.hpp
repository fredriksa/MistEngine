#pragma once

#include "SFML/System/Vector2.hpp"

namespace Core
{
    class SystemsRegistry;
}

namespace sf
{
    class RenderWindow;
}

namespace Core
{
    struct EngineContext
    {
        sf::Vector2u WindowSize;
        std::shared_ptr<sf::RenderWindow> Window;
        std::shared_ptr<SystemsRegistry> SystemsRegistry;
    };
}
