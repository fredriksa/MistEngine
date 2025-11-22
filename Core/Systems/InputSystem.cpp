#include "InputSystem.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "../EngineContext.hpp"

namespace Core
{
    InputSystem::InputSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("InputSystem", Type, std::move(InContext))
    {
    }

    void InputSystem::Tick(float DeltaTime)
    {
        std::shared_ptr<sf::RenderWindow>& Window = GetContext()->Window;
        while (const std::optional event = Window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                Window->close();
        }
    }
}
