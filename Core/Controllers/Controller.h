#pragma once

#include <SFML/Window/Event.hpp>

#include "../Components/Component.h"
#include "../Input/InputSource.h"

namespace Core
{
    class Controller : public Component
    {
    public:
        Controller(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context, InputSource Source, const std::string& TypeName);
        ~Controller() override;

        void Shutdown() override;

        InputSource GetAssignedSource() const { return AssignedSource; }

        virtual void OnMouseButtonPressed(const sf::Event::MouseButtonPressed& Event)
        {
        }

        virtual void OnMouseButtonReleased(const sf::Event::MouseButtonReleased& Event)
        {
        }

        virtual void OnMouseMoved(const sf::Event::MouseMoved& Event)
        {
        }

        virtual void OnMouseWheelScrolled(const sf::Event::MouseWheelScrolled& Event)
        {
        }

        virtual void OnKeyPressed(const sf::Event::KeyPressed& Event)
        {
        }

        virtual void OnKeyReleased(const sf::Event::KeyReleased& Event)
        {
        }

        virtual void OnJoystickButtonPressed(const sf::Event::JoystickButtonPressed& Event)
        {
        }

        virtual void OnJoystickButtonReleased(const sf::Event::JoystickButtonReleased& Event)
        {
        }

        virtual void OnJoystickMoved(const sf::Event::JoystickMoved& Event)
        {
        }

    private:
        InputSource AssignedSource;
    };
}
