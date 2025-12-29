#include "PlayTestScene.h"
#include "SceneLoader.h"
#include "../World/World.h"
#include "../World/WorldObject.h"
#include "../Components/CameraComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/PlayerCharacterComponent.h"
#include "../Components/PlayerSpawnComponent.h"
#include "../Controllers/PlayerControllerComponent.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/SceneManagerSystem.h"
#include "imgui.h"
#include <thread>
#include <chrono>

namespace Core
{
    PlayTestScene::PlayTestScene(std::shared_ptr<EngineContext> InContext, std::string InSceneName)
        : Scene(std::move(InContext), "PlayTest")
          , SceneName(std::move(InSceneName))
    {
    }

    void PlayTestScene::OnLoad()
    {
        Scene::OnLoad();

        SceneLoader Loader(Context);
        Task<std::vector<AssetId>> LoadTask = Loader.LoadScene(SceneName, World);
        while (!LoadTask.await_ready())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        SpawnPlayer();
    }

    void PlayTestScene::PreRender()
    {
        if (PlayerPawn)
        {
            if (std::shared_ptr<CameraComponent> Camera = PlayerPawn->Components().Get<CameraComponent>())
            {
                Context->Renderer->setView(Camera->GetView());
            }
        }
    }

    void PlayTestScene::RenderUI()
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(150, 50));
        ImGui::Begin("PlayTest", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        if (ImGui::Button("Exit (Esc)", ImVec2(-1, 0)) || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            if (std::shared_ptr<SceneManagerSystem> SceneManager = Context->SystemsRegistry->GetCoreSystem<
                SceneManagerSystem>())
            {
                SceneManager->RequestPop();
            }
        }

        ImGui::End();
    }

    void PlayTestScene::SpawnPlayer()
    {
        WorldObject* SpawnPoint = FindPlayerSpawnPoint();
        if (!SpawnPoint)
        {
            CreateDefaultSpawnPoint();
            SpawnPoint = FindPlayerSpawnPoint();
        }

        if (!SpawnPoint)
            return;

        std::shared_ptr<TransformComponent> SpawnTransform = SpawnPoint->Components().Get<TransformComponent>();
        sf::Vector2f SpawnPosition = SpawnTransform ? SpawnTransform->Position : sf::Vector2f(0.0f, 0.0f);

        PlayerController = World.Objects().CreateObject();
        PlayerController->SetName("PlayerController");
        PlayerController->Components().Add<TransformComponent>();
        PlayerController->Components().Add<PlayerControllerComponent>();
        std::shared_ptr<PlayerControllerComponent> Controller = PlayerController->Components().Get<
            PlayerControllerComponent>();

        PlayerPawn = World.Objects().CreateObject();
        PlayerPawn->SetName("PlayerPawn");
        PlayerPawn->Components().Add<TransformComponent>();
        std::shared_ptr<TransformComponent> PawnTransform = PlayerPawn->Components().Get<TransformComponent>();
        PawnTransform->Position = SpawnPosition;
        PlayerPawn->Components().Add<SpriteComponent>();
        PlayerPawn->Components().Add<PlayerCharacterComponent>();
        PlayerPawn->Components().Add<CameraComponent>();
        std::shared_ptr<CameraComponent> Camera = PlayerPawn->Components().Get<CameraComponent>();
        Camera->SetZoom(0.25f);

        Controller->Possess(PlayerPawn);
    }

    WorldObject* PlayTestScene::FindPlayerSpawnPoint()
    {
        const std::vector<std::shared_ptr<WorldObject>>& AllObjects = World.Objects().GetAll();
        for (const std::shared_ptr<WorldObject>& Object : AllObjects)
        {
            if (Object->Components().Get<PlayerSpawnComponent>())
            {
                return Object.get();
            }
        }
        return nullptr;
    }

    void PlayTestScene::CreateDefaultSpawnPoint()
    {
        std::shared_ptr<WorldObject> SpawnPoint = World.Objects().CreateObject();
        SpawnPoint->SetName("DefaultPlayerSpawn");
        SpawnPoint->Components().Add<TransformComponent>();
        std::shared_ptr<TransformComponent> Transform = SpawnPoint->Components().Get<TransformComponent>();
        Transform->Position = sf::Vector2f(0.0f, 0.0f);
        SpawnPoint->Components().Add<PlayerSpawnComponent>();
    }
}
