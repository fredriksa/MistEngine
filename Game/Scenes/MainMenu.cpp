#include "MainMenu.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../../Core/World/WorldObject.h"
#include "../WorldObjects/Barrel.h"

namespace Game
{
    MainMenuScene::MainMenuScene(std::shared_ptr<Core::EngineContext> InContext)
        : Scene(std::move(InContext), "MainMenu")
    {
    }

    void MainMenuScene::OnLoad()
    {
        //World.CreateWorldObject<WorldObjects::Barrel>();
    }

    void MainMenuScene::PreRender()
    {
        sf::RectangleShape BackgroundRect(static_cast<sf::Vector2f>(Context->WindowSize));
        BackgroundRect.setFillColor(sf::Color::Blue);
        BackgroundRect.setPosition(sf::Vector2f(0, 0));
        Context->Window->draw(BackgroundRect);
    }
}
