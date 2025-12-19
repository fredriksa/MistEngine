#include "LevelDesigner.h"

#include "imgui.h"
#include <SFML/Graphics/Sprite.hpp>
#include "../../SystemsRegistry.hpp"
#include "../../Systems/SceneManagerSystem.h"
#include "../../Systems/AssetRegistrySystem.h"

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

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Level", "Ctrl+N"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Open Level...", "Ctrl+O"))
            {
                /* TODO */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Save As..."))
            {
                /* TODO */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit to Main Menu")) { ExitToMainMenu(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y"))
            {
                /* TODO */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear Level"))
            {
                /* TODO */
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Show Grid", "G"))
            {
                /* TODO */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Zoom In", "+"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Zoom Out", "-"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Reset View", "R"))
            {
                /* TODO */
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Select Tool", "V"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Paint Tool", "B"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Fill Tool", "F"))
            {
                /* TODO */
            }
            if (ImGui::MenuItem("Eraser Tool", "E"))
            {
                /* TODO */
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Entity Placer"))
            {
                /* TODO */
            }
            ImGui::EndMenu();
        }

        float MenuBarHeight = ImGui::GetWindowSize().y;
        float BackButtonWidth = 80.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - BackButtonWidth);
        if (ImGui::Button("Back", ImVec2(BackButtonWidth - 10, MenuBarHeight - 4)))
        {
            ExitToMainMenu();
        }

        ImGui::EndMainMenuBar();
    }

    float MenuBarHeight = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(0, MenuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - MenuBarHeight));
    ImGui::Begin("LevelDesignerContent", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    float ContentWidth = ImGui::GetContentRegionAvail().x;
    float ContentHeight = ImGui::GetContentRegionAvail().y;

    if (!bTilePaletteFloating)
    {
        RenderTilePalettePanel();
        ImGui::SameLine();
        RenderTilePaletteDivider();
        ImGui::SameLine();
    }

    RenderCanvasArea();

    if (!bPropertiesFloating)
    {
        ImGui::SameLine();
        RenderPropertiesPanel();
    }

    ImGui::End();

    if (bTilePaletteFloating)
    {
        RenderTilePalettePanel();
    }

    if (bPropertiesFloating)
    {
        RenderPropertiesPanel();
    }
}

void Game::LevelDesignerScene::RenderTilePalettePanel()
{
    bool bIsFloating = bTilePaletteFloating;

    if (bIsFloating)
    {
        if (!ImGui::Begin("Tile Palette", &bTilePaletteFloating))
        {
            ImGui::End();
            return;
        }
    }
    else
    {
        float ContentHeight = ImGui::GetContentRegionAvail().y;
        ImGui::BeginChild("TilePalette", ImVec2(TilePalettePanelWidth, ContentHeight), true);
    }

    ImGui::SeparatorText("Tile Palette");

    if (!bIsFloating)
    {
        ImGui::SameLine();
        float ButtonWidth = 60.0f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ButtonWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::SmallButton("Detach"))
        {
            bTilePaletteFloating = true;
        }
        ImGui::PopStyleVar();
    }

    if (!Context || !Context->SystemsRegistry)
    {
        if (bIsFloating) ImGui::End(); else ImGui::EndChild();
        return;
    }

    std::shared_ptr<Core::AssetRegistrySystem> AssetRegistry = Context->SystemsRegistry->GetCoreSystem<
        Core::AssetRegistrySystem>();
    if (!AssetRegistry)
    {
        if (bIsFloating) ImGui::End(); else ImGui::EndChild();
        return;
    }

    std::vector<std::shared_ptr<const Core::TileSheet>> TileSheets = AssetRegistry->GetAllTileSheets();

    ImGui::Text("Tilesheet:");
    ImGui::SetNextItemWidth(-1);

    if (TileSheets.empty())
    {
        ImGui::TextDisabled("No tilesheets loaded");
    }
    else
    {
        std::string CurrentName = SelectedTileSheetIndex < TileSheets.size()
                                      ? TileSheets[SelectedTileSheetIndex]->GetName()
                                      : "None";

        if (ImGui::BeginCombo("##TileSheetSelect", CurrentName.c_str()))
        {
            for (int i = 0; i < TileSheets.size(); ++i)
            {
                bool bIsSelected = (SelectedTileSheetIndex == i);
                if (ImGui::Selectable(TileSheets[i]->GetName().c_str(), bIsSelected))
                {
                    SelectedTileSheetIndex = i;
                }
                if (bIsSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::BeginChild("TileGrid", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (!TileSheets.empty() && SelectedTileSheetIndex < TileSheets.size())
    {
        const std::shared_ptr<const Core::TileSheet>& SelectedSheet = TileSheets[SelectedTileSheetIndex];
        if (!SelectedSheet)
        {
            ImGui::EndChild();
            if (bIsFloating) ImGui::End(); else ImGui::EndChild();
            return;
        }

        std::shared_ptr<const sf::Texture> Texture = SelectedSheet->GetTexture();
        if (!Texture)
        {
            ImGui::EndChild();
            if (bIsFloating) ImGui::End(); else ImGui::EndChild();
            return;
        }

        const int TilesPerRow = SelectedSheet->GetNumColumns();
        const float TileDisplaySize = 32.0f;
        const Core::uint TileCount = SelectedSheet->GetTileCount();

        sf::Vector2u TextureSize = Texture->getSize();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        for (Core::uint i = 0; i < TileCount; ++i)
        {
            if (i % TilesPerRow != 0)
                ImGui::SameLine();

            ImGui::PushID(i);

            sf::IntRect TileRect = SelectedSheet->GetTileRect(i);

            sf::Sprite TileSprite(*Texture);
            TileSprite.setTextureRect(TileRect);

            bool bIsSelected = (SelectedTileIndex == static_cast<int>(i));

            if (ImGui::ImageButton("##Tile", TileSprite, sf::Vector2f(TileDisplaySize, TileDisplaySize)))
            {
                SelectedTileIndex = static_cast<int>(i);
            }

            bool bIsHovered = ImGui::IsItemHovered();

            if (bIsSelected || bIsHovered)
            {
                ImVec2 Min = ImGui::GetItemRectMin();
                ImVec2 Max = ImGui::GetItemRectMax();
                ImU32 Color = bIsSelected
                                  ? IM_COL32(255, 215, 0, 128)
                                  : IM_COL32(255, 255, 255, 64);
                ImGui::GetWindowDrawList()->AddRectFilled(Min, Max, Color);
            }

            ImGui::PopID();
        }

        ImGui::PopStyleVar(2);
    }
    else
    {
        ImGui::TextDisabled("Select a tilesheet to view tiles");
    }

    ImGui::EndChild();
    if (bIsFloating) ImGui::End(); else ImGui::EndChild();
}

void Game::LevelDesignerScene::RenderTilePaletteDivider()
{
    float ContentHeight = ImGui::GetContentRegionAvail().y;

    ImGui::Button("##PaletteDivider", ImVec2(4.0f, ContentHeight));
    if (ImGui::IsItemActive())
    {
        TilePalettePanelWidth += ImGui::GetIO().MouseDelta.x;
        TilePalettePanelWidth = std::max(200.0f, std::min(TilePalettePanelWidth, 800.0f));
    }
    if (ImGui::IsItemHovered())
    {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    }
}

void Game::LevelDesignerScene::RenderCanvasArea()
{
    float ContentHeight = ImGui::GetContentRegionAvail().y;
    float CanvasWidth = ImGui::GetContentRegionAvail().x;

    if (!bPropertiesFloating)
    {
        CanvasWidth -= PropertiesPanelWidth + 4.0f;
    }

    ImGui::BeginChild("Canvas", ImVec2(CanvasWidth, ContentHeight), true);
    ImGui::EndChild();
}

void Game::LevelDesignerScene::RenderPropertiesPanel()
{
    bool bIsFloating = bPropertiesFloating;

    if (bIsFloating)
    {
        if (!ImGui::Begin("Properties", &bPropertiesFloating))
        {
            ImGui::End();
            return;
        }
    }
    else
    {
        float ContentHeight = ImGui::GetContentRegionAvail().y;

        ImGui::Button("##Divider", ImVec2(4.0f, ContentHeight));
        if (ImGui::IsItemActive())
        {
            PropertiesPanelWidth -= ImGui::GetIO().MouseDelta.x;
            PropertiesPanelWidth = std::max(200.0f, std::min(PropertiesPanelWidth, 600.0f));
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        ImGui::SameLine();

        ImGui::BeginChild("Properties", ImVec2(PropertiesPanelWidth - 4.0f, ContentHeight), true);
    }

    ImGui::SeparatorText("Properties");

    if (!bIsFloating)
    {
        ImGui::SameLine();
        float ButtonWidth = 60.0f;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - ButtonWidth);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        if (ImGui::SmallButton("Detach"))
        {
            bPropertiesFloating = true;
        }
        ImGui::PopStyleVar();
    }

    // Placeholder content
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Selected Tile: None");
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Position");
        ImGui::InputFloat("X##Pos", &PlaceholderPosX);
        ImGui::InputFloat("Y##Pos", &PlaceholderPosY);
        ImGui::Spacing();
        ImGui::Text("Rotation");
        ImGui::SliderFloat("Angle", &PlaceholderRotation, 0.0f, 360.0f);
        ImGui::Spacing();
        ImGui::Text("Scale");
        ImGui::InputFloat("X##Scale", &PlaceholderScaleX);
        ImGui::InputFloat("Y##Scale", &PlaceholderScaleY);
    }

    if (ImGui::CollapsingHeader("Sprite Component"))
    {
        ImGui::Text("Texture: [None]");
        if (ImGui::Button("Select Texture..."))
        {
            // TODO: Open texture picker
        }
        ImGui::Spacing();
        static bool bVisible = true;
        ImGui::Checkbox("Visible", &bVisible);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::Button("Add Component", ImVec2(-1, 0)))
    {
        // TODO: Show component picker
    }

    if (ImGui::Button("Remove Component", ImVec2(-1, 0)))
    {
        // TODO: Remove selected component
    }

    if (bIsFloating) ImGui::End(); else ImGui::EndChild();
}

void Game::LevelDesignerScene::ExitToMainMenu()
{
    Context->SystemsRegistry->GetCoreSystem<
        Core::SceneManagerSystem>()->RequestPop();
}
