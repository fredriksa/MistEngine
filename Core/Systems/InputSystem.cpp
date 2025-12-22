#include "InputSystem.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <algorithm>

#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/ImGuiSystem.h"
#include "../Controllers/Controller.h"
#include "../Input/InputSource.h"

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
            {
                Window->close();
            }

            for (Controller* Ctrl : Controllers)
            {
                if (!Ctrl) continue;

                InputSource Source = Ctrl->GetAssignedSource();

                if (event->is<sf::Event::MouseButtonPressed>())
                {
                    if (Source == InputSource::KeyboardMouse)
                    {
                        if (const auto* e = event->getIf<sf::Event::MouseButtonPressed>())
                            Ctrl->OnMouseButtonPressed(*e);
                    }
                }
                else if (event->is<sf::Event::MouseButtonReleased>())
                {
                    if (Source == InputSource::KeyboardMouse)
                    {
                        if (const auto* e = event->getIf<sf::Event::MouseButtonReleased>())
                            Ctrl->OnMouseButtonReleased(*e);
                    }
                }
                else if (event->is<sf::Event::MouseMoved>())
                {
                    if (Source == InputSource::KeyboardMouse)
                    {
                        if (const auto* e = event->getIf<sf::Event::MouseMoved>())
                            Ctrl->OnMouseMoved(*e);
                    }
                }
                else if (event->is<sf::Event::MouseWheelScrolled>())
                {
                    if (Source == InputSource::KeyboardMouse)
                    {
                        if (const auto* e = event->getIf<sf::Event::MouseWheelScrolled>())
                            Ctrl->OnMouseWheelScrolled(*e);
                    }
                }
                else if (event->is<sf::Event::KeyPressed>())
                {
                    if (Source == InputSource::KeyboardMouse)
                    {
                        if (const auto* e = event->getIf<sf::Event::KeyPressed>())
                            Ctrl->OnKeyPressed(*e);
                    }
                }
                else if (event->is<sf::Event::KeyReleased>())
                {
                    if (Source == InputSource::KeyboardMouse)
                    {
                        if (const auto* e = event->getIf<sf::Event::KeyReleased>())
                            Ctrl->OnKeyReleased(*e);
                    }
                }
                else if (event->is<sf::Event::JoystickButtonPressed>())
                {
                    if (const auto* e = event->getIf<sf::Event::JoystickButtonPressed>())
                    {
                        if ((e->joystickId == 0 && Source == InputSource::Gamepad0) ||
                            (e->joystickId == 1 && Source == InputSource::Gamepad1))
                        {
                            Ctrl->OnJoystickButtonPressed(*e);
                        }
                    }
                }
                else if (event->is<sf::Event::JoystickButtonReleased>())
                {
                    if (const auto* e = event->getIf<sf::Event::JoystickButtonReleased>())
                    {
                        if ((e->joystickId == 0 && Source == InputSource::Gamepad0) ||
                            (e->joystickId == 1 && Source == InputSource::Gamepad1))
                        {
                            Ctrl->OnJoystickButtonReleased(*e);
                        }
                    }
                }
                else if (event->is<sf::Event::JoystickMoved>())
                {
                    if (const auto* e = event->getIf<sf::Event::JoystickMoved>())
                    {
                        if ((e->joystickId == 0 && Source == InputSource::Gamepad0) ||
                            (e->joystickId == 1 && Source == InputSource::Gamepad1))
                        {
                            Ctrl->OnJoystickMoved(*e);
                        }
                    }
                }
            }
        }
    }

    void InputSystem::RegisterController(Controller* InController)
    {
        if (InController)
        {
            Controllers.push_back(InController);
        }
    }

    void InputSystem::UnregisterController(Controller* InController)
    {
        Controllers.erase(
            std::remove(Controllers.begin(), Controllers.end(), InController),
            Controllers.end()
        );
    }
}
