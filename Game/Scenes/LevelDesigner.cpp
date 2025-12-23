#include "LevelDesigner.h"

#include "imgui.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>
#include <unordered_map>
#include "../../Components/CameraComponent.h"
#include "../../SystemsRegistry.hpp"
#include "../../Systems/SceneManagerSystem.h"
#include "../../Systems/AssetRegistrySystem.h"
#include "../../Core/Editor/EditorConstants.h"
#include "../../Core/TileMap/TileSheet.h"

namespace Game
{
    LevelDesignerScene::LevelDesignerScene(std::shared_ptr<Core::EngineContext> InContext)
        : Scene(std::move(InContext), "LevelDesigner")
    {
    }

    void LevelDesignerScene::OnLoad()
    {
        Scene::OnLoad();
    }

    void LevelDesignerScene::PreRender()
{
    Scene::PreRender();

    CanvasRect = CalculateCanvasRect();

    if (std::shared_ptr<Core::WorldObject> CameraObject = World.GetObjectByName("EditorCamera"))
    {
        if (std::shared_ptr<Core::CameraComponent> Camera = CameraObject->Components().Get<Core::CameraComponent>())
        {
            Context->Window->setView(Camera->GetView());

            int ScissorX = static_cast<int>(CanvasRect.position.x);
            int ScissorY = static_cast<int>(Context->WindowSize.y - CanvasRect.position.y - CanvasRect.size.y);
            int ScissorWidth = static_cast<int>(CanvasRect.size.x);
            int ScissorHeight = static_cast<int>(CanvasRect.size.y);

            glEnable(GL_SCISSOR_TEST);
            glScissor(ScissorX, ScissorY, ScissorWidth, ScissorHeight);
        }
    }
}

void LevelDesignerScene::PostRender()
{
    glDisable(GL_SCISSOR_TEST);

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
    ImGui::SetNextWindowBgAlpha(0.0f);
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

void LevelDesignerScene::RenderTilePalettePanel()
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
        if (bIsFloating) ImGui::End();
        else ImGui::EndChild();
        return;
    }

    std::shared_ptr<Core::AssetRegistrySystem> AssetRegistry = Context->SystemsRegistry->GetCoreSystem<
        Core::AssetRegistrySystem>();
    if (!AssetRegistry)
    {
        if (bIsFloating) ImGui::End();
        else ImGui::EndChild();
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
        std::string CurrentName = CurrentTileSheetIndex >= 0 && CurrentTileSheetIndex < static_cast<int>(TileSheets.size())
                                      ? TileSheets[CurrentTileSheetIndex]->GetName()
                                      : "None";

        if (ImGui::BeginCombo("##TileSheetSelect", CurrentName.c_str()))
        {
            for (int i = 0; i < TileSheets.size(); ++i)
            {
                bool bIsSelected = (CurrentTileSheetIndex == i);
                if (ImGui::Selectable(TileSheets[i]->GetName().c_str(), bIsSelected))
                {
                    OnTileSheetChanged(i);
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

    if (!TileSheets.empty() && CurrentTileSheetIndex >= 0 && CurrentTileSheetIndex < static_cast<int>(TileSheets.size()))
    {
        const std::shared_ptr<const Core::TileSheet>& SelectedSheet = TileSheets[CurrentTileSheetIndex];
        if (!SelectedSheet)
        {
            ImGui::EndChild();
            if (bIsFloating) ImGui::End();
            else ImGui::EndChild();
            return;
        }

        std::shared_ptr<const sf::Texture> Texture = SelectedSheet->GetTexture();
        if (!Texture)
        {
            ImGui::EndChild();
            if (bIsFloating) ImGui::End();
            else ImGui::EndChild();
            return;
        }

        const int TilesPerRow = SelectedSheet->GetNumColumns();
        const Core::uint TileCount = SelectedSheet->GetTileCount();

        sf::Vector2u TextureSize = Texture->getSize();

        std::unordered_map<int, Core::ScreenRect> TileScreenRects;

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

        for (Core::uint i = 0; i < TileCount; ++i)
        {
            int Column = i % TilesPerRow;
            int Row = i / TilesPerRow;

            if (Column != 0)
                ImGui::SameLine();

            ImGui::PushID(i);

            sf::IntRect TileRect = SelectedSheet->GetTileRect(i);
            sf::Sprite TileSprite(*Texture);
            TileSprite.setTextureRect(TileRect);

            ImVec2 TileScreenPos = ImGui::GetCursorScreenPos();
            TileScreenRects[i] = Core::ScreenRect(
                Core::ScreenCoordinate::FromImGui(TileScreenPos),
                Core::ScreenCoordinate(Core::EditorConstants::TilePaletteDisplaySize, Core::EditorConstants::TilePaletteDisplaySize)
            );

            ImGui::Image(TileSprite,
                sf::Vector2f(Core::EditorConstants::TilePaletteDisplaySize, Core::EditorConstants::TilePaletteDisplaySize));

            bool bIsHovered = ImGui::IsItemHovered();

            if (bIsHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                bIsDraggingTileSelection = true;
                DragStartColumn = Column;
                DragStartRow = Row;
                CurrentSelection.TileSheetIndex = CurrentTileSheetIndex;
                CurrentSelection.SelectionRect = Core::TileRectCoord(
                    Core::TileCoordinate(Column, Row),
                    Core::TileCoordinate(1, 1)
                );
            }

            if (bIsHovered && bIsDraggingTileSelection)
            {
                int MinCol = std::min(DragStartColumn, Column);
                int MaxCol = std::max(DragStartColumn, Column);
                int MinRow = std::min(DragStartRow, Row);
                int MaxRow = std::max(DragStartRow, Row);

                CurrentSelection.SelectionRect = Core::TileRectCoord(
                    Core::TileCoordinate(MinCol, MinRow),
                    Core::TileCoordinate(MaxCol - MinCol + 1, MaxRow - MinRow + 1)
                );
            }

            if (bIsHovered && !bIsDraggingTileSelection)
            {
                ImVec2 Min = ImGui::GetItemRectMin();
                ImVec2 Max = ImGui::GetItemRectMax();
                ImGui::GetWindowDrawList()->AddRectFilled(Min, Max, IM_COL32(255, 255, 255, 64));
            }

            ImGui::PopID();
        }

        if (bIsDraggingTileSelection && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            bIsDraggingTileSelection = false;
        }

        if (CurrentSelection.IsValid() && CurrentSelection.TileSheetIndex == CurrentTileSheetIndex)
        {
            for (int y = 0; y < CurrentSelection.SelectionRect.Height(); ++y)
            {
                for (int x = 0; x < CurrentSelection.SelectionRect.Width(); ++x)
                {
                    int Column = CurrentSelection.SelectionRect.Position.X() + x;
                    int Row = CurrentSelection.SelectionRect.Position.Y() + y;
                    int TileIndex = Row * TilesPerRow + Column;

                    if (TileScreenRects.find(TileIndex) != TileScreenRects.end())
                    {
                        const Core::ScreenRect& Rect = TileScreenRects[TileIndex];
                        ImVec2 Min = ImVec2(Rect.Left(), Rect.Top());
                        ImVec2 Max = ImVec2(Rect.Right(), Rect.Bottom());

                        ImGui::GetWindowDrawList()->AddRect(
                            Min, Max,
                            IM_COL32(255, 215, 0, 255),
                            0.0f, 0, 2.0f
                        );
                    }
                }
            }
        }

        ImGui::PopStyleVar(2);
    }
    else
    {
        ImGui::TextDisabled("Select a tilesheet to view tiles");
    }

    ImGui::EndChild();
    if (bIsFloating) ImGui::End();
    else ImGui::EndChild();
}

void LevelDesignerScene::RenderTilePaletteDivider()
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

void LevelDesignerScene::RenderCanvasArea()
{
    float ContentHeight = ImGui::GetContentRegionAvail().y;
    float CanvasWidth = ImGui::GetContentRegionAvail().x;

    if (!bPropertiesFloating)
    {
        CanvasWidth -= PropertiesPanelWidth + 4.0f;
    }

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::BeginChild("Canvas", ImVec2(CanvasWidth, ContentHeight), true);
    ImGui::EndChild();
    ImGui::PopStyleColor();
}

void LevelDesignerScene::RenderPropertiesPanel()
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

    if (bIsFloating) ImGui::End();
    else ImGui::EndChild();
}

void LevelDesignerScene::ExitToMainMenu()
{
    Context->SystemsRegistry->GetCoreSystem<
        Core::SceneManagerSystem>()->RequestPop();
}

bool LevelDesignerScene::IsClickInCanvas(Core::WindowCoordinate MousePos) const
{
    return CanvasRect.contains(sf::Vector2f(static_cast<float>(MousePos.X()), static_cast<float>(MousePos.Y())));
}

sf::FloatRect LevelDesignerScene::CalculateCanvasRect() const
{
    float MenuBarHeight = ImGui::GetFrameHeight();
    float CanvasX = bTilePaletteFloating ? 0.0f : TilePalettePanelWidth + 4.0f;
    float CanvasY = MenuBarHeight;
    float CanvasWidth = ImGui::GetIO().DisplaySize.x - CanvasX;
    float CanvasHeight = ImGui::GetIO().DisplaySize.y - MenuBarHeight;

    if (!bPropertiesFloating)
    {
        CanvasWidth -= PropertiesPanelWidth + 4.0f;
    }

    return sf::FloatRect{{CanvasX, CanvasY}, {CanvasWidth, CanvasHeight}};
}

int LevelDesignerScene::GetTileSheetColumns(int TileSheetIndex) const
{
    if (!Context || !Context->SystemsRegistry)
        return 0;

    std::shared_ptr<Core::AssetRegistrySystem> AssetRegistry =
        Context->SystemsRegistry->GetCoreSystem<Core::AssetRegistrySystem>();
    if (!AssetRegistry)
        return 0;

    std::vector<std::shared_ptr<const Core::TileSheet>> TileSheets = AssetRegistry->GetAllTileSheets();
    if (TileSheetIndex < 0 || TileSheetIndex >= static_cast<int>(TileSheets.size()))
        return 0;

    return TileSheets[TileSheetIndex]->GetNumColumns();
}

void LevelDesignerScene::OnTileSheetChanged(int NewTileSheetIndex)
{
    CurrentTileSheetIndex = NewTileSheetIndex;
    CurrentSelection = TileSelection();
}
}
