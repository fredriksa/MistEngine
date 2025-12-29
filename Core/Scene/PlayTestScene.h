#pragma once

#include "Scene.h"
#include <memory>
#include <string>
#include <SFML/Window/Event.hpp>

namespace Core
{
    class WorldObject;
    struct EngineContext;

    class PlayTestScene : public Scene
    {
    public:
        PlayTestScene(std::shared_ptr<EngineContext> InContext, std::string InSceneName);

        void OnLoad() override;
        void PreRender() override;
        void RenderUI() override;

    private:
        void SpawnPlayer();
        WorldObject* FindPlayerSpawnPoint();
        void CreateDefaultSpawnPoint();

        std::string SceneName;
        std::shared_ptr<WorldObject> PlayerController;
        std::shared_ptr<WorldObject> PlayerPawn;
    };
}
