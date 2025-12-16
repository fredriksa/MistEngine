#include "InputSystem.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/ImGuiSystem.h"

namespace Core
{
    InputSystem::InputSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("InputSystem", Type, std::move(InContext))
    {
    }

    void InputSystem::Start()
    {
        ImGuiSystemPtr = GetContext()->SystemsRegistry->GetCoreSystem<ImGuiSystem>();
    }

    void InputSystem::Tick(float DeltaTime)
    {
        std::shared_ptr<sf::RenderWindow>& Window = GetContext()->Window;
        while (const std::optional event = Window->pollEvent())
        {
            if (ImGuiSystemPtr)
            {
                ImGuiSystemPtr->ProcessEvent(*event);
            }

            if (event->is<sf::Event::Closed>())
                Window->close();
        }
    }
}
