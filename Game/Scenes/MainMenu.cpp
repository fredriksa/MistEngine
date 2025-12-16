#include "MainMenu.h"

#include "../../SystemsRegistry.hpp"
#include "../../Systems/SceneManagerSystem.h"

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

    void MainMenuScene::PreRender()
    {
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
}
