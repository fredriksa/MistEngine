#include "LevelDesignerView.h"
#include "LevelDesignerViewModel.h"
#include "LevelDesignerModel.h"
#include "ObjectSelection.h"
#include "EditorConstants.h"
#include "ComponentEditorRegistry.h"
#include "UITheme.h"
#include "../Components/CameraComponent.h"
#include "../Components/TileMapComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/ComponentRegistry.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/AssetRegistrySystem.h"
#include "../Systems/SceneManagerSystem.h"
#include "../Scene/PlayTestScene.h"
#include "../TileMap/TileSheet.h"
#include "../TileMap/TileMap.h"
#include "../World/World.h"
#include "../World/WorldObject.h"
#include "../World/WorldEnvironment.h"
#include "../World/ComponentManager.h"
#include "imgui.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <SFML/Graphics/RenderWindow.hpp>

namespace Core
{
    LevelDesignerView::LevelDesignerView(LevelDesignerViewModel& InViewModel)
        : ViewModel(InViewModel)
    {
    }

    void LevelDesignerView::RenderMenuBar(std::optional<Task<>>& LoadingTask)
    {
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, UITheme::MenuBarBackground);
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    ViewModel.NewScene();
                    ViewModel.SelectObject(nullptr);
                }
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                {
                    bShowOpenSceneModal = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    ViewModel.SaveScene();
                }
                if (ImGui::MenuItem("Save As..."))
                {
                    bShowSaveAsModal = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit to Main Menu"))
                {
                    ViewModel.RequestExitToMainMenu();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                const bool bShowGrid = ViewModel.IsGridVisible();
                if (ImGui::MenuItem("Show Grid", "G", bShowGrid))
                {
                    ViewModel.ToggleGrid();
                }
                ImGui::EndMenu();
            }

            const ImVec2 EnvironmentTextSize = ImGui::CalcTextSize("Environment");
            const ImVec2 EnvironmentPos = ImGui::GetCursorScreenPos();
            const float MenuHeight = ImGui::GetFrameHeight();

            ImGui::Selectable("Environment", bShowEnvironmentPanel, ImGuiSelectableFlags_None, ImVec2(EnvironmentTextSize.x, MenuHeight));

            if (ImGui::IsItemClicked())
            {
                EnvironmentButtonPos = ImVec2(EnvironmentPos.x, EnvironmentPos.y + MenuHeight);
                bShowEnvironmentPanel = !bShowEnvironmentPanel;
            }

            const float MenuBarHeight = ImGui::GetWindowSize().y;
            const float PlayButtonWidth = 80.0f;
            const float BackButtonWidth = 80.0f;
            const float TotalButtonWidth = PlayButtonWidth + BackButtonWidth;

            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - TotalButtonWidth);

            const bool bCanPlayTest = ViewModel.CanPlayTest();
            if (!bCanPlayTest)
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button("Play", ImVec2(PlayButtonWidth - 10, MenuBarHeight - 4)))
            {
                ViewModel.SaveScene();

                const std::string SceneName = ViewModel.GetModel().GetCurrentScene().GetName();
                if (!SceneName.empty())
                {
                    if (std::shared_ptr<SceneManagerSystem> SceneManager = ViewModel.GetModel().GetContext()->SystemsRegistry->GetCoreSystem<SceneManagerSystem>())
                    {
                        SceneManager->Push<PlayTestScene>(SceneName);
                    }
                }
            }

            if (!bCanPlayTest && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("Save the scene first (File > Save)");
            }

            if (!bCanPlayTest)
            {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (ImGui::Button("Back", ImVec2(BackButtonWidth - 10, MenuBarHeight - 4)))
            {
                ViewModel.RequestExitToMainMenu();
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleColor();

        RenderSaveAsModal();
        RenderOpenSceneModal(LoadingTask);
    }

    void LevelDesignerView::RenderSaveAsModal()
    {
        if (bShowSaveAsModal)
        {
            ImGui::OpenPopup("Save Scene As");
            bShowSaveAsModal = false;
        }

        const ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(Center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Save Scene As", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Enter scene name:");
            ImGui::InputText("##scenename", SaveAsSceneNameBuffer, sizeof(SaveAsSceneNameBuffer));

            ImGui::Spacing();

            if (ImGui::Button("Save", ImVec2(120, 0)))
            {
                if (strlen(SaveAsSceneNameBuffer) > 0)
                {
                    ViewModel.SaveSceneAs(SaveAsSceneNameBuffer);
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void LevelDesignerView::RenderOpenSceneModal(std::optional<Task<>>& LoadingTask)
    {
        if (bShowOpenSceneModal)
        {
            AvailableScenes = ViewModel.GetModel().GetAvailableScenes();
            ImGui::OpenPopup("Open Scene");
            bShowOpenSceneModal = false;
        }

        const ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(Center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("Open Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Select a scene to open:");
            ImGui::Spacing();

            ImGui::BeginChild("SceneList", ImVec2(400, 300), true);
            for (int i = 0; i < static_cast<int>(AvailableScenes.size()); ++i)
            {
                const bool bIsSelected = (SelectedSceneIndex == i);
                if (ImGui::Selectable(AvailableScenes[i].c_str(), bIsSelected, ImGuiSelectableFlags_AllowDoubleClick))
                {
                    SelectedSceneIndex = i;
                    if (ImGui::IsMouseDoubleClicked(0))
                    {
                        LoadingTask = ViewModel.LoadScene(AvailableScenes[SelectedSceneIndex]);
                        ImGui::CloseCurrentPopup();
                    }
                }
            }
            ImGui::EndChild();

            ImGui::Spacing();

            if (ImGui::Button("Open", ImVec2(120, 0)))
            {
                if (SelectedSceneIndex >= 0 && SelectedSceneIndex < static_cast<int>(AvailableScenes.size()))
                {
                    LoadingTask = ViewModel.LoadScene(AvailableScenes[SelectedSceneIndex]);
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void LevelDesignerView::RenderMainContent()
    {
        const float MenuBarHeight = ImGui::GetFrameHeight();

        ImGui::SetNextWindowPos(ImVec2(0, MenuBarHeight));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - MenuBarHeight));
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("LevelDesignerContent", nullptr,
                     ImGuiWindowFlags_NoTitleBar |
                     ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoInputs);

        const bool bHasTileMap = ViewModel.GetSelectedTileMap() != nullptr;

        if (bHasTileMap)
        {
            RenderTilePalettePanel();
            ImGui::SameLine();
            RenderTilePaletteDivider();
            ImGui::SameLine();
        }

        RenderCanvasArea();

        ImGui::SameLine();
        RenderPropertiesPanel();

        RenderEnvironmentPanel();

        ImGui::End();
    }

    void LevelDesignerView::CalculateCanvasRect()
    {
        const float MenuBarHeight = ImGui::GetFrameHeight();
        const sf::Vector2u WindowSize = ViewModel.GetWindowSize();
        const float WindowWidth = static_cast<float>(WindowSize.x);
        const float WindowHeight = static_cast<float>(WindowSize.y);

        float Left = 0.0f;
        float Top = MenuBarHeight;
        float Width = WindowWidth;
        float Height = WindowHeight - MenuBarHeight;

        const bool bHasTileMap = ViewModel.GetSelectedTileMap() != nullptr;
        if (bHasTileMap)
        {
            Left += ViewModel.GetTilePalettePanelWidth() + 4.0f;
            Width -= ViewModel.GetTilePalettePanelWidth() + 4.0f;
        }

        Width -= ViewModel.GetPropertiesPanelWidth() + 4.0f;

        CanvasRect = sf::FloatRect(sf::Vector2f(Left, Top), sf::Vector2f(Width, Height));
    }

    void LevelDesignerView::RenderTilePalettePanel()
    {
        const float ContentHeight = ImGui::GetContentRegionAvail().y;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, UITheme::PanelBackground);
        ImGui::BeginChild("TilePalette", ImVec2(ViewModel.GetTilePalettePanelWidth(), ContentHeight), true);
        ImGui::PopStyleColor();

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            ViewModel.SetMouseOverBlockingUI(true);
        }

        ImGui::SeparatorText("Tile Palette");

        const std::vector<std::shared_ptr<const TileSheet>> TileSheets = ViewModel.GetAllTileSheets();

        ImGui::Text("Tilesheet:");
        ImGui::SetNextItemWidth(-1);

        if (TileSheets.empty())
        {
            ImGui::TextDisabled("No tilesheets loaded");
        }
        else
        {
            const int CurrentIndex = ViewModel.GetCurrentTileSheetIndex();
            const std::string CurrentName = CurrentIndex >= 0 && CurrentIndex < static_cast<int>(TileSheets.size())
                                                ? TileSheets[CurrentIndex]->GetName()
                                                : "None";

            if (ImGui::BeginCombo("##TileSheetSelect", CurrentName.c_str()))
            {
                for (int i = 0; i < static_cast<int>(TileSheets.size()); ++i)
                {
                    const bool bIsSelected = (CurrentIndex == i);
                    if (ImGui::Selectable(TileSheets[i]->GetName().c_str(), bIsSelected))
                    {
                        ViewModel.SetTileSheetIndex(i);
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

        const int CurrentIndex = ViewModel.GetCurrentTileSheetIndex();
        if (!TileSheets.empty() && CurrentIndex >= 0 && CurrentIndex < static_cast<int>(TileSheets.size()))
        {
            const std::shared_ptr<const TileSheet>& SelectedSheet = TileSheets[CurrentIndex];
            if (SelectedSheet)
            {
                const std::shared_ptr<const sf::Texture> Texture = SelectedSheet->GetTexture();
                if (Texture)
                {
                    const int TilesPerRow = SelectedSheet->GetNumColumns();
                    const uint TileCount = SelectedSheet->GetTileCount();

                    std::unordered_map<int, ScreenRect> TileScreenRects;

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

                    for (uint i = 0; i < TileCount; ++i)
                    {
                        const int Column = i % TilesPerRow;
                        const int Row = i / TilesPerRow;

                        if (Column != 0)
                            ImGui::SameLine();

                        ImGui::PushID(i);

                        const sf::IntRect TileRect = SelectedSheet->GetTileRect(i);
                        sf::Sprite TileSprite(*Texture);
                        TileSprite.setTextureRect(TileRect);

                        const ImVec2 TileScreenPos = ImGui::GetCursorScreenPos();
                        TileScreenRects[i] = ScreenRect(
                            ScreenCoordinate::FromImGui(TileScreenPos),
                            ScreenCoordinate(EditorConstants::TilePaletteDisplaySize,
                                             EditorConstants::TilePaletteDisplaySize)
                        );

                        ImGui::Image(TileSprite,
                                     sf::Vector2f(EditorConstants::TilePaletteDisplaySize,
                                                  EditorConstants::TilePaletteDisplaySize));

                        const bool bIsHovered = ImGui::IsItemHovered();

                        if (bIsHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                        {
                            bIsDraggingTileSelection = true;
                            DragStartColumn = Column;
                            DragStartRow = Row;
                            ViewModel.SelectTile(CurrentIndex,
                                                 TileRectCoord(TileCoordinate(Column, Row), TileCoordinate(1, 1)));
                            ViewModel.SetCurrentTool(EditorTool::Brush);
                        }

                        if (bIsHovered && bIsDraggingTileSelection)
                        {
                            const int MinCol = std::min(DragStartColumn, Column);
                            const int MaxCol = std::max(DragStartColumn, Column);
                            const int MinRow = std::min(DragStartRow, Row);
                            const int MaxRow = std::max(DragStartRow, Row);

                            ViewModel.SelectTile(CurrentIndex,
                                                 TileRectCoord(TileCoordinate(MinCol, MinRow),
                                                               TileCoordinate(
                                                                   MaxCol - MinCol + 1, MaxRow - MinRow + 1)));
                        }

                        if (bIsHovered && !bIsDraggingTileSelection)
                        {
                            const ImVec2 Min = ImGui::GetItemRectMin();
                            const ImVec2 Max = ImGui::GetItemRectMax();
                            ImGui::GetWindowDrawList()->AddRectFilled(Min, Max, IM_COL32(255, 255, 255, 64));
                        }

                        ImGui::PopID();
                    }

                    if (bIsDraggingTileSelection && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                    {
                        bIsDraggingTileSelection = false;
                    }

                    const TileSelection CurrentSelection = ViewModel.GetCurrentSelection();
                    if (CurrentSelection.TileSheetIndex.has_value() &&
                        CurrentSelection.TileSheetIndex.value() == static_cast<uint>(CurrentIndex) &&
                        CurrentSelection.SelectionRect.Width() > 0 &&
                        CurrentSelection.SelectionRect.Height() > 0)
                    {
                        for (int y = 0; y < CurrentSelection.SelectionRect.Height(); ++y)
                        {
                            for (int x = 0; x < CurrentSelection.SelectionRect.Width(); ++x)
                            {
                                const int Column = CurrentSelection.SelectionRect.Position.X() + x;
                                const int Row = CurrentSelection.SelectionRect.Position.Y() + y;
                                const int TileIndex = Row * TilesPerRow + Column;

                                if (TileScreenRects.find(TileIndex) != TileScreenRects.end())
                                {
                                    const ScreenRect& Rect = TileScreenRects[TileIndex];
                                    const ImVec2 Min = ImVec2(Rect.Left(), Rect.Top());
                                    const ImVec2 Max = ImVec2(Rect.Right(), Rect.Bottom());

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
            }
        }

        ImGui::EndChild();
        ImGui::EndChild();
    }

    void LevelDesignerView::RenderTilePaletteDivider()
    {
        const float ContentHeight = ImGui::GetContentRegionAvail().y;

        ImGui::Button("##PaletteDivider", ImVec2(4.0f, ContentHeight));
        if (ImGui::IsItemActive())
        {
            float Width = ViewModel.GetTilePalettePanelWidth() + ImGui::GetIO().MouseDelta.x;
            Width = std::max(200.0f, std::min(Width, 800.0f));
            ViewModel.SetTilePalettePanelWidth(Width);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }
    }

    void LevelDesignerView::RenderCanvasArea()
    {
        const float ContentHeight = ImGui::GetContentRegionAvail().y;
        float CanvasWidth = ImGui::GetContentRegionAvail().x;

        CanvasWidth -= ViewModel.GetPropertiesPanelWidth() + 4.0f;

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::BeginChild("Canvas", ImVec2(CanvasWidth, ContentHeight), ImGuiChildFlags_None);

        RenderCanvasToolbar();

        if (ViewModel.IsLayersPanelOpen())
        {
            RenderLayersPanel();
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void LevelDesignerView::RenderCanvasToolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

        const bool bHasTileMap = ViewModel.GetSelectedTileMap() != nullptr;
        const bool bHasTileSelection = ViewModel.GetCurrentSelection().IsValid();
        const EditorTool CurrentTool = ViewModel.GetCurrentTool();

        const bool bIsTileEditingTool = (CurrentTool == EditorTool::Brush ||
                                          CurrentTool == EditorTool::Fill ||
                                          CurrentTool == EditorTool::Eraser ||
                                          CurrentTool == EditorTool::Eyedropper);

        if (!bHasTileMap && bIsTileEditingTool)
        {
            ViewModel.SetCurrentTool(EditorTool::Select);
        }

        if (bHasTileMap)
        {
            if (ImGui::Button("Layers"))
            {
                ViewModel.SetLayersPanelOpen(!ViewModel.IsLayersPanelOpen());
            }

            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::SameLine();

            if (CurrentTool == EditorTool::Brush)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
            }
            if (ImGui::Button("Brush"))
            {
                ViewModel.ToggleTool(EditorTool::Brush);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Brush Tool (B)");
            }
            if (CurrentTool == EditorTool::Brush)
            {
                ImGui::PopStyleColor(3);
            }

            ImGui::SameLine();
            if (CurrentTool == EditorTool::Fill)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
            }
            if (ImGui::Button("Fill"))
            {
                ViewModel.ToggleTool(EditorTool::Fill);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Fill Tool (F)");
            }
            if (CurrentTool == EditorTool::Fill)
            {
                ImGui::PopStyleColor(3);
            }

            ImGui::SameLine();
            if (CurrentTool == EditorTool::Eraser)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
            }
            if (ImGui::Button("Eraser"))
            {
                ViewModel.ToggleTool(EditorTool::Eraser);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Eraser Tool (D)");
            }
            if (CurrentTool == EditorTool::Eraser)
            {
                ImGui::PopStyleColor(3);
            }

            ImGui::SameLine();
            if (CurrentTool == EditorTool::Eyedropper)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
            }
            if (ImGui::Button("Eyedropper"))
            {
                ViewModel.ToggleTool(EditorTool::Eyedropper);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Eyedropper Tool (E)");
            }
            if (CurrentTool == EditorTool::Eyedropper)
            {
                ImGui::PopStyleColor(3);
            }
        }

        ImGui::PopStyleVar(2);
        ImGui::Separator();
    }

    void LevelDesignerView::RenderLayersPanel()
    {
        const std::shared_ptr<TileMapComponent> TileMapComp = ViewModel.GetSelectedTileMap();
        if (!TileMapComp)
        {
            ImGui::TextDisabled("No tilemap selected");
            return;
        }

        TileMap& TileMapData = TileMapComp->GetTileMap();
        const uint LayerCount = TileMapData.GetLayerCount();

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
        ImGui::SetNextWindowSize(ImVec2(250, 400));

        bool bPanelOpen = ViewModel.IsLayersPanelOpen();
        ImGui::Begin("Layers Panel", &bPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        ViewModel.SetLayersPanelOpen(bPanelOpen);

        ImGui::SeparatorText("Layers");

        ImGui::BeginChild("LayerList", ImVec2(0, -30), true);

        for (int i = static_cast<int>(LayerCount) - 1; i >= 0; --i)
        {
            ImGui::PushID(i);

            const bool bIsSelected = (ViewModel.GetCurrentLayer() == static_cast<uint>(i));

            if (bIsSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
            }

            ImGui::BeginGroup();

            bool bIsVisible = TileMapComp->IsLayerVisible(static_cast<uint>(i));
            if (ImGui::Checkbox(("##Visible" + std::to_string(i)).c_str(), &bIsVisible))
            {
                TileMapComp->SetLayerVisible(static_cast<uint>(i), bIsVisible);
            }
            ImGui::SameLine();

            const std::string LayerName = "Layer " + std::to_string(i);
            if (ImGui::Button(LayerName.c_str(), ImVec2(-1, 0)))
            {
                ViewModel.SetCurrentLayer(static_cast<uint>(i));
            }

            ImGui::EndGroup();

            if (bIsSelected)
            {
                ImGui::PopStyleColor(3);
            }

            ImGui::PopID();
        }

        ImGui::EndChild();

        ImGui::Separator();

        ImGui::BeginGroup();
        if (ImGui::Button("+", ImVec2(50, 0)))
        {
            TileMapData.AddLayer();
            ViewModel.SetCurrentLayer(LayerCount);
        }
        ImGui::SameLine();
        if (ImGui::Button("-", ImVec2(50, 0)))
        {
            if (LayerCount > 1)
            {
                TileMapData.RemoveLayer(ViewModel.GetCurrentLayer());
                if (ViewModel.GetCurrentLayer() >= TileMapData.GetLayerCount())
                {
                    ViewModel.SetCurrentLayer(TileMapData.GetLayerCount() - 1);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("^", ImVec2(50, 0)))
        {
            if (ViewModel.GetCurrentLayer() < LayerCount - 1)
            {
                TileMapData.SwapLayers(ViewModel.GetCurrentLayer(), ViewModel.GetCurrentLayer() + 1);
                ViewModel.SetCurrentLayer(ViewModel.GetCurrentLayer() + 1);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("v", ImVec2(50, 0)))
        {
            if (ViewModel.GetCurrentLayer() > 0)
            {
                TileMapData.SwapLayers(ViewModel.GetCurrentLayer(), ViewModel.GetCurrentLayer() - 1);
                ViewModel.SetCurrentLayer(ViewModel.GetCurrentLayer() - 1);
            }
        }
        ImGui::EndGroup();

        ImGui::End();
    }

    void LevelDesignerView::RenderPropertiesPanel()
    {
        const float ContentHeight = ImGui::GetContentRegionAvail().y;

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            ViewModel.SetMouseOverBlockingUI(true);
        }

        ImGui::Button("##Divider", ImVec2(4.0f, ContentHeight));
        if (ImGui::IsItemActive())
        {
            float Width = ViewModel.GetPropertiesPanelWidth() - ImGui::GetIO().MouseDelta.x;
            Width = std::max(200.0f, std::min(Width, 600.0f));
            ViewModel.SetPropertiesPanelWidth(Width);
        }
        if (ImGui::IsItemHovered())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_ChildBg, UITheme::PanelBackground);
        ImGui::BeginChild("Properties", ImVec2(ViewModel.GetPropertiesPanelWidth() - 4.0f, ContentHeight), true);
        ImGui::PopStyleColor();

        RenderSceneHierarchy();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        RenderObjectProperties();

        ImGui::EndChild();
    }

    void LevelDesignerView::RenderSceneHierarchy()
    {
        ImGui::SeparatorText("Scene Hierarchy");

        const float HierarchyHeight = ImGui::GetContentRegionAvail().y * 0.4f;
        ImGui::BeginChild("HierarchyList", ImVec2(0, HierarchyHeight), true);

        const std::vector<std::shared_ptr<WorldObject>> GameObjects = ViewModel.GetGameObjects();
        const std::shared_ptr<WorldObject> SelectedPtr = ViewModel.GetSelectedObject();

        for (size_t i = 0; i < GameObjects.size(); ++i)
        {
            const std::shared_ptr<WorldObject>& Obj = GameObjects[i];
            if (!Obj)
                continue;

            const bool bIsSelected = ViewModel.IsObjectSelected(Obj.get());
            const std::string ObjectName = ViewModel.GetObjectDisplayName(Obj.get());

            ImGui::PushID(static_cast<int>(i));

            const float ButtonWidth = 20.0f;
            const float AvailableWidth = ImGui::GetContentRegionAvail().x;

            if (ImGui::Selectable(ObjectName.c_str(), bIsSelected, 0, ImVec2(AvailableWidth - ButtonWidth * 2 - 8, 0)))
            {
                ViewModel.SelectObject(Obj);
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                {
                    ViewModel.DeleteSelectedObject();
                }
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("^"))
            {
                ViewModel.MoveSelectedObjectUp();
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("v"))
            {
                ViewModel.MoveSelectedObjectDown();
            }

            ImGui::PopID();
        }

        if (ImGui::BeginPopupContextWindow("HierarchyContextMenu",
                                           ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
        {
            if (ImGui::MenuItem("Create Object"))
            {
                ViewModel.CreateNewObject();
            }

            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }

    void LevelDesignerView::RenderObjectProperties()
    {
        ImGui::SeparatorText("Properties");

        const std::shared_ptr<WorldObject> SelectedPtr = ViewModel.GetSelectedObject();

        if (!SelectedPtr)
        {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No object selected");
            EditingObjectName.clear();
            return;
        }

        if (EditingObjectName != SelectedPtr->GetName())
        {
            EditingObjectName = SelectedPtr->GetName();
        }

        EditingObjectName.resize(256);

        ImGui::Text("Name");
        if (ImGui::InputText("##ObjectName", EditingObjectName.data(), EditingObjectName.capacity()))
        {
            EditingObjectName = EditingObjectName.c_str();
            SelectedPtr->SetName(EditingObjectName);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        std::vector<Component*> ComponentsToRemove;

        {
            const std::unordered_map<std::type_index, std::shared_ptr<Component>>& AllComponents =
                SelectedPtr->Components().GetAll();

            for (const std::pair<const std::type_index, std::shared_ptr<Component>>& ComponentPair : AllComponents)
            {
                Component* Comp = ComponentPair.second.get();
                if (!Comp)
                    continue;

                IComponentEditor* Editor = ComponentEditorRegistry::Get().GetEditor(Comp);
                if (Editor)
                {
                    Editor->OnRemoveRequested = [&ComponentsToRemove](Component* CompToRemove)
                    {
                        ComponentsToRemove.push_back(CompToRemove);
                    };

                    Editor->RenderEditor(Comp);
                }
            }
        }

        for (Component* Comp : ComponentsToRemove)
        {
            SelectedPtr->Components().Remove(Comp);
        }

        ImGui::Spacing();

        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
        {
            bShowAddComponentModal = true;
        }

        if (bShowAddComponentModal)
        {
            ImGui::OpenPopup("Add Component");
            bShowAddComponentModal = false;
        }

        if (ImGui::BeginPopupModal("Add Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            const std::vector<std::string> AvailableComponents = ComponentRegistry::Get().GetRegisteredTypeNames();

            ImGui::Text("Select component type:");
            ImGui::SetNextItemWidth(300);

            const std::string CurrentSelection = SelectedComponentTypeIndex >= 0 &&
                                                 SelectedComponentTypeIndex < static_cast<int>(AvailableComponents.
                                                     size())
                                                     ? AvailableComponents[SelectedComponentTypeIndex]
                                                     : "";

            if (ImGui::BeginCombo("##ComponentType", CurrentSelection.c_str()))
            {
                for (int i = 0; i < static_cast<int>(AvailableComponents.size()); ++i)
                {
                    const bool bIsSelected = (SelectedComponentTypeIndex == i);
                    if (ImGui::Selectable(AvailableComponents[i].c_str(), bIsSelected))
                    {
                        SelectedComponentTypeIndex = i;
                    }
                    if (bIsSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::Spacing();

            if (ImGui::Button("Add", ImVec2(120, 0)))
            {
                if (SelectedComponentTypeIndex >= 0 &&
                    SelectedComponentTypeIndex < static_cast<int>(AvailableComponents.size()))
                {
                    const std::string& ComponentName = AvailableComponents[SelectedComponentTypeIndex];
                    const std::shared_ptr<Component> NewComponent =
                        ComponentRegistry::Get().Create(ComponentName, SelectedPtr, SelectedPtr->GetContextPtr());

                    if (NewComponent)
                    {
                        SelectedPtr->Components().Attach(NewComponent);
                    }
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void LevelDesignerView::DrawSceneGrid()
    {
        if (!ViewModel.IsGridVisible())
            return;

        const std::shared_ptr<WorldObject> CameraObject = ViewModel.GetWorld().Objects().GetByName(
            "EditorCamera");
        if (!CameraObject)
            return;

        const std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>();
        if (!Camera)
            return;

        const sf::View View = Camera->GetView();
        const sf::Vector2f ViewCenter = View.getCenter();
        const sf::Vector2f ViewSize = View.getSize();

        const float Left = ViewCenter.x - ViewSize.x / 2.0f;
        const float Right = ViewCenter.x + ViewSize.x / 2.0f;
        const float Top = ViewCenter.y - ViewSize.y / 2.0f;
        const float Bottom = ViewCenter.y + ViewSize.y / 2.0f;

        const float GridSize = 16.0f;

        const int StartX = static_cast<int>(std::floor(Left / GridSize));
        const int EndX = static_cast<int>(std::ceil(Right / GridSize));
        const int StartY = static_cast<int>(std::floor(Top / GridSize));
        const int EndY = static_cast<int>(std::ceil(Bottom / GridSize));

        sf::VertexArray Lines(sf::PrimitiveType::Lines);
        const sf::Color GridColor(200, 200, 200, 80);
        const sf::Color OriginColor(255, 100, 100, 200);

        for (int x = StartX; x <= EndX; ++x)
        {
            const float XPos = static_cast<float>(x) * GridSize;
            const sf::Color LineColor = (x == 0) ? OriginColor : GridColor;
            Lines.append(sf::Vertex(sf::Vector2f(XPos, Top), LineColor));
            Lines.append(sf::Vertex(sf::Vector2f(XPos, Bottom), LineColor));
        }

        for (int y = StartY; y <= EndY; ++y)
        {
            const float YPos = static_cast<float>(y) * GridSize;
            const sf::Color LineColor = (y == 0) ? OriginColor : GridColor;
            Lines.append(sf::Vertex(sf::Vector2f(Left, YPos), LineColor));
            Lines.append(sf::Vertex(sf::Vector2f(Right, YPos), LineColor));
        }

        ViewModel.GetRenderer().draw(Lines);
    }

    void LevelDesignerView::RenderGizmos()
    {
        const std::shared_ptr<WorldObject> CameraObject = ViewModel.GetWorld().Objects().GetByName(
            "EditorCamera");
        if (!CameraObject)
            return;

        const std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>();
        if (!Camera)
            return;

        WorldObject* HoveredObject = ViewModel.GetModel().GetHoveredObject();
        const ObjectSelection& Selection = ViewModel.GetModel().GetSelection();

        sf::VertexArray Gizmos(sf::PrimitiveType::Lines);
        const sf::Color HoverColor(200, 200, 200, 255);
        const sf::Color SelectionColor(255, 215, 0, 255);

        if (HoveredObject && !Selection.Contains(HoveredObject))
        {
            const TransformComponent* Transform = HoveredObject->Transform();
            if (Transform)
            {
                const sf::Vector2f Position = Transform->Position;
                constexpr float BoundsSize = 10.0f;
                constexpr float HalfSize = BoundsSize / 2.0f;

                const sf::Vector2f TopLeft(Position.x - HalfSize, Position.y - HalfSize);
                const sf::Vector2f TopRight(Position.x + HalfSize, Position.y - HalfSize);
                const sf::Vector2f BottomLeft(Position.x - HalfSize, Position.y + HalfSize);
                const sf::Vector2f BottomRight(Position.x + HalfSize, Position.y + HalfSize);

                Gizmos.append(sf::Vertex(TopLeft, HoverColor));
                Gizmos.append(sf::Vertex(TopRight, HoverColor));

                Gizmos.append(sf::Vertex(TopRight, HoverColor));
                Gizmos.append(sf::Vertex(BottomRight, HoverColor));

                Gizmos.append(sf::Vertex(BottomRight, HoverColor));
                Gizmos.append(sf::Vertex(BottomLeft, HoverColor));

                Gizmos.append(sf::Vertex(BottomLeft, HoverColor));
                Gizmos.append(sf::Vertex(TopLeft, HoverColor));
            }
        }

        const std::vector<WorldObject*> ObjectsInSelectionRect = ViewModel.GetObjectsInCurrentSelectionRectangle();
        for (WorldObject* Object : ObjectsInSelectionRect)
        {
            if (!Object || Selection.Contains(Object))
                continue;

            const TransformComponent* Transform = Object->Transform();
            if (!Transform)
                continue;

            const sf::Vector2f Position = Transform->Position;
            constexpr float BoundsSize = 10.0f;
            constexpr float HalfSize = BoundsSize / 2.0f;

            const sf::Vector2f TopLeft(Position.x - HalfSize, Position.y - HalfSize);
            const sf::Vector2f TopRight(Position.x + HalfSize, Position.y - HalfSize);
            const sf::Vector2f BottomLeft(Position.x - HalfSize, Position.y + HalfSize);
            const sf::Vector2f BottomRight(Position.x + HalfSize, Position.y + HalfSize);

            Gizmos.append(sf::Vertex(TopLeft, HoverColor));
            Gizmos.append(sf::Vertex(TopRight, HoverColor));

            Gizmos.append(sf::Vertex(TopRight, HoverColor));
            Gizmos.append(sf::Vertex(BottomRight, HoverColor));

            Gizmos.append(sf::Vertex(BottomRight, HoverColor));
            Gizmos.append(sf::Vertex(BottomLeft, HoverColor));

            Gizmos.append(sf::Vertex(BottomLeft, HoverColor));
            Gizmos.append(sf::Vertex(TopLeft, HoverColor));
        }

        const std::vector<std::shared_ptr<WorldObject>> SelectedObjects = Selection.GetAllValid();
        for (const std::shared_ptr<WorldObject>& Object : SelectedObjects)
        {
            const TransformComponent* Transform = Object->Transform();
            if (!Transform)
                continue;

            const sf::Vector2f Position = Transform->Position;
            constexpr float BoundsSize = 10.0f;
            constexpr float HalfSize = BoundsSize / 2.0f;

            const sf::Vector2f TopLeft(Position.x - HalfSize, Position.y - HalfSize);
            const sf::Vector2f TopRight(Position.x + HalfSize, Position.y - HalfSize);
            const sf::Vector2f BottomLeft(Position.x - HalfSize, Position.y + HalfSize);
            const sf::Vector2f BottomRight(Position.x + HalfSize, Position.y + HalfSize);

            Gizmos.append(sf::Vertex(TopLeft, SelectionColor));
            Gizmos.append(sf::Vertex(TopRight, SelectionColor));

            Gizmos.append(sf::Vertex(TopRight, SelectionColor));
            Gizmos.append(sf::Vertex(BottomRight, SelectionColor));

            Gizmos.append(sf::Vertex(BottomRight, SelectionColor));
            Gizmos.append(sf::Vertex(BottomLeft, SelectionColor));

            Gizmos.append(sf::Vertex(BottomLeft, SelectionColor));
            Gizmos.append(sf::Vertex(TopLeft, SelectionColor));

            constexpr float HandleRadius = 2.0f;
            constexpr int HandleSegments = 8;
            for (int i = 0; i < HandleSegments; ++i)
            {
                const float Angle1 = (static_cast<float>(i) / static_cast<float>(HandleSegments)) * 2.0f * 3.14159f;
                const float Angle2 = (static_cast<float>(i + 1) / static_cast<float>(HandleSegments)) * 2.0f * 3.14159f;
                const sf::Vector2f Point1(Position.x + std::cos(Angle1) * HandleRadius,
                                          Position.y + std::sin(Angle1) * HandleRadius);
                const sf::Vector2f Point2(Position.x + std::cos(Angle2) * HandleRadius,
                                          Position.y + std::sin(Angle2) * HandleRadius);
                Gizmos.append(sf::Vertex(Point1, SelectionColor));
                Gizmos.append(sf::Vertex(Point2, SelectionColor));
            }

            constexpr float ArrowLength = 20.0f;
            constexpr float ArrowHeadSize = 5.0f;
            const sf::Color XAxisColor(255, 0, 0, 255);
            const sf::Color YAxisColor(0, 255, 0, 255);

            const sf::Vector2f XAxisEnd(Position.x + ArrowLength, Position.y);
            Gizmos.append(sf::Vertex(Position, XAxisColor));
            Gizmos.append(sf::Vertex(XAxisEnd, XAxisColor));
            Gizmos.append(sf::Vertex(XAxisEnd, XAxisColor));
            Gizmos.append(sf::Vertex(sf::Vector2f(XAxisEnd.x - ArrowHeadSize, XAxisEnd.y - ArrowHeadSize), XAxisColor));
            Gizmos.append(sf::Vertex(XAxisEnd, XAxisColor));
            Gizmos.append(sf::Vertex(sf::Vector2f(XAxisEnd.x - ArrowHeadSize, XAxisEnd.y + ArrowHeadSize), XAxisColor));

            const sf::Vector2f YAxisEnd(Position.x, Position.y + ArrowLength);
            Gizmos.append(sf::Vertex(Position, YAxisColor));
            Gizmos.append(sf::Vertex(YAxisEnd, YAxisColor));
            Gizmos.append(sf::Vertex(YAxisEnd, YAxisColor));
            Gizmos.append(sf::Vertex(sf::Vector2f(YAxisEnd.x - ArrowHeadSize, YAxisEnd.y - ArrowHeadSize), YAxisColor));
            Gizmos.append(sf::Vertex(YAxisEnd, YAxisColor));
            Gizmos.append(sf::Vertex(sf::Vector2f(YAxisEnd.x + ArrowHeadSize, YAxisEnd.y - ArrowHeadSize), YAxisColor));
        }

        ViewModel.GetRenderer().draw(Gizmos);

        if (ViewModel.IsSelectingRectangle())
        {
            const WindowCoordinate SelectStart = ViewModel.GetSelectionRectStart();
            const WindowCoordinate SelectCurrent = ViewModel.GetSelectionRectCurrent();

            const sf::Vector2u RenderSize = ViewModel.GetWindowSize();
            sf::View DefaultView;
            DefaultView.setSize(sf::Vector2f(static_cast<float>(RenderSize.x), static_cast<float>(RenderSize.y)));
            DefaultView.setCenter(sf::Vector2f(static_cast<float>(RenderSize.x) / 2.0f, static_cast<float>(RenderSize.y) / 2.0f));
            ViewModel.GetRenderer().setView(DefaultView);

            const sf::Color OutlineColor(100, 150, 255, 255);

            const float MinX = static_cast<float>(std::min(SelectStart.X(), SelectCurrent.X()));
            const float MaxX = static_cast<float>(std::max(SelectStart.X(), SelectCurrent.X()));
            const float MinY = static_cast<float>(std::min(SelectStart.Y(), SelectCurrent.Y()));
            const float MaxY = static_cast<float>(std::max(SelectStart.Y(), SelectCurrent.Y()));

            const sf::Vector2f TopLeft(MinX, MinY);
            const sf::Vector2f TopRight(MaxX, MinY);
            const sf::Vector2f BottomLeft(MinX, MaxY);
            const sf::Vector2f BottomRight(MaxX, MaxY);

            sf::VertexArray RectOutline(sf::PrimitiveType::Lines);
            RectOutline.append(sf::Vertex(TopLeft, OutlineColor));
            RectOutline.append(sf::Vertex(TopRight, OutlineColor));
            RectOutline.append(sf::Vertex(TopRight, OutlineColor));
            RectOutline.append(sf::Vertex(BottomRight, OutlineColor));
            RectOutline.append(sf::Vertex(BottomRight, OutlineColor));
            RectOutline.append(sf::Vertex(BottomLeft, OutlineColor));
            RectOutline.append(sf::Vertex(BottomLeft, OutlineColor));
            RectOutline.append(sf::Vertex(TopLeft, OutlineColor));

            ViewModel.GetRenderer().draw(RectOutline);

            ViewModel.GetRenderer().setView(Camera->GetView());
        }
    }

    void LevelDesignerView::RenderEnvironmentPanel()
    {
        if (!bShowEnvironmentPanel)
        {
            return;
        }

        ImGui::SetNextWindowPos(EnvironmentButtonPos, ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_FirstUseEver);

        ImGui::Begin("Environment Panel", &bShowEnvironmentPanel, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
        {
            ViewModel.SetMouseOverBlockingUI(true);
        }

        WorldEnvironment& Env = ViewModel.GetWorld().GetEnvironment();
        WorldTime& Time = Env.GetTime();
        DayNightColors& Colors = Env.GetDayNightColors();

        ImGui::SeparatorText("Time");

        int Hours = Time.GetHourOfDay();
        int Minutes = Time.GetMinutes();
        bool bStaticTime = Time.IsStaticTime();
        float TimeScale = Time.GetTimeScale();

        if (ImGui::SliderInt("Hours", &Hours, 0, 23))
        {
            Time.SetTime(Hours, Minutes);
        }

        if (ImGui::SliderInt("Minutes", &Minutes, 0, 59))
        {
            Time.SetTime(Hours, Minutes);
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Settings");

        if (ImGui::Checkbox("Static Time", &bStaticTime))
        {
            Time.SetStaticTime(bStaticTime);
        }

        if (ImGui::SliderFloat("Time Scale", &TimeScale, 0.1f, 10.0f, "%.1fx"))
        {
            Time.SetTimeScale(TimeScale);
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Preview");

        ViewModel.UpdateTimePreview();

        float PreviewDuration = ViewModel.GetModel().GetTimePreviewDuration();
        if (ImGui::SliderFloat("Duration (s)", &PreviewDuration, 1.0f, 60.0f, "%.1f"))
        {
            ViewModel.SetTimePreviewDuration(PreviewDuration);
        }

        if (ViewModel.GetModel().IsPreviewingTime())
        {
            const float Progress = ViewModel.GetModel().GetTimePreviewProgress();
            ImGui::ProgressBar(Progress, ImVec2(-1, 0));

            if (ImGui::Button("Stop Preview", ImVec2(-1, 0)))
            {
                ViewModel.StopTimePreview();
            }
        }
        else
        {
            if (ImGui::Button("Preview", ImVec2(-1, 0)))
            {
                ViewModel.StartTimePreview();
            }
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Colors");

        ImGui::Text("Night (21:00 - 5:00)");
        ImGui::ColorEdit3("##Night", &Colors.Night.x);

        ImGui::Spacing();
        ImGui::Text("Sunrise (5:00 - 7:00)");
        ImGui::ColorEdit3("##Sunrise", &Colors.Sunrise.x);

        ImGui::Spacing();
        ImGui::Text("Day (7:00 - 17:00)");
        ImGui::ColorEdit3("##Day", &Colors.Day.x);

        ImGui::Spacing();
        ImGui::Text("Sunset (17:00 - 19:00)");
        ImGui::ColorEdit3("##Sunset", &Colors.Sunset.x);

        ImGui::End();
    }
}
