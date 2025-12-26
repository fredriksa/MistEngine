#include "MainMenu.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include "../../SystemsRegistry.hpp"
#include "../../Systems/SceneManagerSystem.h"
#include "../../Core/Components/CameraComponent.h"
#include "../../Core/Components/TileMapComponent.h"
#include "../../Core/Components/TransformComponent.h"
#include "../../Core/World/WorldConstants.h"

#include "imgui.h"
#include "LevelDesigner.h"

namespace Core
{
    class SceneManagerSystem;
}

namespace Game
{
    MainMenuScene::MainMenuScene(std::shared_ptr<Core::EngineContext> InContext)
        : Scene(std::move(InContext), "MainMenu")
    {
    }

    void MainMenuScene::OnLoad()
    {
    }

    void MainMenuScene::OnEnter()
    {
        ZoomCameraToFitTileMap();
    }

    void MainMenuScene::PreRender()
    {
        for (const auto& Obj : World.Objects().GetAll())
        {
            if (!Obj)
                continue;

            if (std::shared_ptr<Core::CameraComponent> Camera = Obj->Components().Get<Core::CameraComponent>())
            {
                Context->Window->setView(Camera->GetView());
                break;
            }
        }
    }

    void MainMenuScene::PostRender()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

        ImGui::Begin("MainMenu", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

        float WindowWidth = ImGui::GetIO().DisplaySize.x;
        float WindowHeight = ImGui::GetIO().DisplaySize.y;
        float ButtonWidth = 350.0f;
        float ButtonHeight = 80.0f;
        float Spacing = 30.0f;

        float StartX = (WindowWidth - ButtonWidth) * 0.5f;
        float StartY = (WindowHeight - (ButtonHeight * 3 + Spacing * 2)) * 0.5f;

        ImGui::SetCursorPos(ImVec2(StartX, StartY));
        if (ImGui::Button("Play", ImVec2(ButtonWidth, ButtonHeight)))
        {
        }

        ImGui::SetCursorPos(ImVec2(StartX, StartY + ButtonHeight + Spacing));
        if (ImGui::Button("Level Designer", ImVec2(ButtonWidth, ButtonHeight)))
        {
            EnterLevelDesigner();
        }

        ImGui::SetCursorPos(ImVec2(StartX, StartY + (ButtonHeight + Spacing) * 2));
        if (ImGui::Button("Quit", ImVec2(ButtonWidth, ButtonHeight)))
        {
            Shutdown();
        }

        ImGui::End();
    }

    void MainMenuScene::EnterLevelDesigner()
    {
        Context->SystemsRegistry->GetCoreSystem<Core::SceneManagerSystem>()->Push<Game::LevelDesignerScene>();
    }

    void MainMenuScene::Shutdown()
    {
        Context->Engine->Shutdown();
    }

    void MainMenuScene::ZoomCameraToFitTileMap()
    {
        std::shared_ptr<Core::WorldObject> CameraObject;
        std::shared_ptr<Core::TileMapComponent> TileMap;

        for (const auto& Obj : World.Objects().GetAll())
        {
            if (!Obj)
                continue;

            if (!CameraObject && Obj->Components().Get<Core::CameraComponent>())
                CameraObject = Obj;

            if (!TileMap)
                TileMap = Obj->Components().Get<Core::TileMapComponent>();

            if (CameraObject && TileMap)
                break;
        }

        if (!CameraObject || !TileMap)
            return;

        std::shared_ptr<Core::CameraComponent> Camera = CameraObject->Components().Get<Core::CameraComponent>();
        std::shared_ptr<Core::TransformComponent> CameraTransform = CameraObject->Components().Get<
            Core::TransformComponent>();

        if (!Camera || !CameraTransform)
            return;

        Core::TileMapComponent::TileBounds Bounds = TileMap->GetValidTileBounds();

        if (!Bounds.IsValid)
            return;

        float BoundsWidthTiles = static_cast<float>(Bounds.MaxX - Bounds.MinX + 1);
        float BoundsHeightTiles = static_cast<float>(Bounds.MaxY - Bounds.MinY + 1);

        float BoundsWidthPixels = BoundsWidthTiles * Core::WorldConstants::TileSize;
        float BoundsHeightPixels = BoundsHeightTiles * Core::WorldConstants::TileSize;

        float WindowWidth = static_cast<float>(Context->WindowSize.x);
        float WindowHeight = static_cast<float>(Context->WindowSize.y);

        float ZoomX = BoundsWidthPixels / WindowWidth;
        float ZoomY = BoundsHeightPixels / WindowHeight;

        float Zoom = (ZoomX > ZoomY) ? ZoomX : ZoomY;

        Camera->SetZoom(Zoom);

        sf::Vector2f TileMapPosition(0.0f, 0.0f);
        if (Core::WorldObject* TileMapOwner = TileMap->GetOwner())
        {
            if (std::shared_ptr<Core::TransformComponent> TileMapTransform = TileMapOwner->Components().Get<
                Core::TransformComponent>())
            {
                TileMapPosition = TileMapTransform->Position;
            }
        }

        float CenterX = TileMapPosition.x + (Bounds.MinX + Bounds.MaxX) * 0.5f * Core::WorldConstants::TileSize +
            Core::WorldConstants::TileSize * 0.5f;
        float CenterY = TileMapPosition.y + (Bounds.MinY + Bounds.MaxY) * 0.5f * Core::WorldConstants::TileSize +
            Core::WorldConstants::TileSize * 0.5f;

        CameraTransform->Position = sf::Vector2f(CenterX, CenterY);
    }
}
