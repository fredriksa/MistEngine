#include "MainMenu.h"

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
        World.CreateWorldObject<WorldObjects::Barrel>();
    }
}
