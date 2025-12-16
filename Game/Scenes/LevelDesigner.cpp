#include "LevelDesigner.h"

Game::LevelDesignerScene::LevelDesignerScene(std::shared_ptr<Core::EngineContext> InContext)
    : Scene(std::move(InContext), "LevelDesigner")
{
}

void Game::LevelDesignerScene::OnLoad()
{
    Scene::OnLoad();
}

void Game::LevelDesignerScene::PreRender()
{
    Scene::PreRender();
}

void Game::LevelDesignerScene::PostRender()
{
    Scene::PostRender();
}
