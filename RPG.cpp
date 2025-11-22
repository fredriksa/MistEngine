#include "Core/Engine.h"
#include "Game/Scenes/MainMenu.h"

int main()
{
    Core::Engine Engine;
    Engine.SetInitialScene<Game::MainMenuScene>();
    Engine.Run();
    return 0;
}
