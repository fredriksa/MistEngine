#include "LevelDesigner.h"

#include "imgui.h"
#include <cmath>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/OpenGL.hpp>
#include <unordered_map>
#include "../../Components/CameraComponent.h"
#include "../../Components/TileMapComponent.h"
#include "../../Components/TransformComponent.h"
#include "../../Controllers/LevelEditorController.h"
#include "../../SystemsRegistry.hpp"
#include "../../Systems/SceneManagerSystem.h"
#include "../../Systems/AssetRegistrySystem.h"
#include <fstream>
#include "../../Core/Editor/EditorConstants.h"
#include "../../Core/Editor/ComponentEditorRegistry.h"
#include "../../Core/TileMap/TileSheet.h"
#include "../../Core/TileMap/TileMap.h"
#include "../../Core/World/ComponentManager.h"

namespace Game
{
    void SceneInfo::SetPath(const std::string& InPath)
    {
        Path = InPath;

        size_t LastSlash = InPath.find_last_of("/\\");
        size_t LastDot = InPath.find_last_of('.');

        if (LastSlash != std::string::npos && LastDot != std::string::npos && LastDot > LastSlash)
        {
            Name = InPath.substr(LastSlash + 1, LastDot - LastSlash - 1);
        }
        else if (LastDot != std::string::npos)
        {
            Name = InPath.substr(0, LastDot);
        }
        else
        {
            Name = InPath;
        }
    }

    const std::string& SceneInfo::GetName() const
    {
        return Name;
    }

    const std::string& SceneInfo::GetPath() const
    {
        return Path;
    }

    bool SceneInfo::IsValid() const
    {
        return !Path.empty();
    }

    void SceneInfo::Clear()
    {
        Name.clear();
        Path.clear();
    }

    LevelDesignerScene::LevelDesignerScene(std::shared_ptr<Core::EngineContext> InContext)
        : Scene(std::move(InContext), "LevelDesigner")
    {
    }

    void LevelDesignerScene::OnLoad()
    {
        Scene::OnLoad();
        CreateEditorInfrastructure();
    }

    void LevelDesignerScene::CreateEditorInfrastructure()
    {
        std::shared_ptr<Core::WorldObject> CameraObj = World.CreateObject();
        CameraObj->SetName("EditorCamera");
        CameraObj->SetTag(Core::ObjectTag::Editor);
        CameraObj->Components().Add<Core::TransformComponent>();
        Core::CameraComponent* Camera = CameraObj->Components().Add<Core::CameraComponent>();
        Camera->SetZoom(0.25f);
        CameraObj->Components().Add<LevelEditorController>();

        // std::shared_ptr<Core::WorldObject> TileMapObj = World.CreateObject();
        // TileMapObj->SetName("TileMap");
        // TileMapObj->SetTag(Core::ObjectTag::Game);
        // Core::TileMapComponent* TileMapComp = TileMapObj->Components().Add<Core::TileMapComponent>();
        // TileMapComp->SetSize(100, 100);
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
        DrawSceneGrid();

        glDisable(GL_SCISSOR_TEST);

        Scene::PostRender();

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New Scene", "Ctrl+N"))
                {
                    World.ClearGameObjects();
                    SelectedObject.reset();
                    CurrentScene.Clear();
                }
                if (ImGui::MenuItem("Open Scene...", "Ctrl+O"))
                {
                    /* TODO */
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                    SaveScene();
                }
                if (ImGui::MenuItem("Save As..."))
                {
                    bShowSaveAsModal = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit to Main Menu")) { ExitToMainMenu(); }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Show Grid", "G", bShowGrid))
                {
                    bShowGrid = !bShowGrid;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Zoom In", "+"))
                {
                    ZoomIn();
                }
                if (ImGui::MenuItem("Zoom Out", "-"))
                {
                    ZoomOut();
                }
                if (ImGui::MenuItem("Reset View", "R"))
                {
                    ResetView();
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Delete Tile", "D"))
                {
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

        if (bShowSaveAsModal)
        {
            ImGui::OpenPopup("Save Scene As");
            bShowSaveAsModal = false;
        }

        ImVec2 Center = ImGui::GetMainViewport()->GetCenter();
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
                    SaveSceneAs(SaveAsSceneNameBuffer);
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

        bool bHasTileMap = GetSelectedTileMap() != nullptr;

        if (!bTilePaletteFloating && bHasTileMap)
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

        if (bTilePaletteFloating && bHasTileMap)
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
            std::string CurrentName = CurrentTileSheetIndex >= 0 && CurrentTileSheetIndex < static_cast<int>(TileSheets.
                                          size())
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

        if (!TileSheets.empty() && CurrentTileSheetIndex >= 0 && CurrentTileSheetIndex < static_cast<int>(TileSheets.
            size()))
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
                    Core::ScreenCoordinate(Core::EditorConstants::TilePaletteDisplaySize,
                                           Core::EditorConstants::TilePaletteDisplaySize)
                );

                ImGui::Image(TileSprite,
                             sf::Vector2f(Core::EditorConstants::TilePaletteDisplaySize,
                                          Core::EditorConstants::TilePaletteDisplaySize));

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

            if (CurrentSelection.TileSheetIndex.has_value() &&
                CurrentSelection.TileSheetIndex.value() == static_cast<Core::uint>(CurrentTileSheetIndex) &&
                CurrentSelection.SelectionRect.Width() > 0 &&
                CurrentSelection.SelectionRect.Height() > 0)
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

        RenderCanvasToolbar();

        if (bLayersPanelOpen)
        {
            RenderLayersPanel();
        }

        ImGui::EndChild();
        ImGui::PopStyleColor();
    }

    void LevelDesignerScene::RenderCanvasToolbar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

        bool bHasTileMap = GetSelectedTileMap() != nullptr;

        if (!bHasTileMap)
        {
            ImGui::BeginDisabled();
        }

        if (ImGui::Button("Layers"))
        {
            bLayersPanelOpen = !bLayersPanelOpen;
        }

        if (!bHasTileMap)
        {
            ImGui::EndDisabled();
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            {
                ImGui::SetTooltip("No tilemap selected");
            }
        }

        ImGui::PopStyleVar(2);
        ImGui::Separator();
    }

    void LevelDesignerScene::RenderLayersPanel()
    {
        std::shared_ptr<Core::TileMapComponent> TileMapComp = GetSelectedTileMap();
        if (!TileMapComp)
        {
            ImGui::TextDisabled("No tilemap selected");
            return;
        }

        Core::TileMap& TileMapData = TileMapComp->GetTileMap();
        Core::uint LayerCount = TileMapData.GetLayerCount();

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
        ImGui::SetNextWindowSize(ImVec2(250, 400));
        ImGui::Begin("Layers Panel", &bLayersPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

        ImGui::SeparatorText("Layers");

        ImGui::BeginChild("LayerList", ImVec2(0, -30), true);

        for (int i = static_cast<int>(LayerCount) - 1; i >= 0; --i)
        {
            ImGui::PushID(i);

            bool bIsSelected = (CurrentLayer == static_cast<Core::uint>(i));

            if (bIsSelected)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.6f, 0.9f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
            }

            ImGui::BeginGroup();

            bool bIsVisible = TileMapComp->IsLayerVisible(static_cast<Core::uint>(i));
            if (ImGui::Checkbox(("##Visible" + std::to_string(i)).c_str(), &bIsVisible))
            {
                TileMapComp->SetLayerVisible(static_cast<Core::uint>(i), bIsVisible);
            }
            ImGui::SameLine();

            std::string LayerName = "Layer " + std::to_string(i);
            if (ImGui::Button(LayerName.c_str(), ImVec2(-1, 0)))
            {
                CurrentLayer = static_cast<Core::uint>(i);
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
            CurrentLayer = LayerCount;
        }
        ImGui::SameLine();
        if (ImGui::Button("-", ImVec2(50, 0)))
        {
            if (LayerCount > 1)
            {
                TileMapData.RemoveLayer(CurrentLayer);
                if (CurrentLayer >= TileMapData.GetLayerCount())
                {
                    CurrentLayer = TileMapData.GetLayerCount() - 1;
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("^", ImVec2(50, 0)))
        {
            if (CurrentLayer < LayerCount - 1)
            {
                TileMapData.SwapLayers(CurrentLayer, CurrentLayer + 1);
                CurrentLayer++;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("v", ImVec2(50, 0)))
        {
            if (CurrentLayer > 0)
            {
                TileMapData.SwapLayers(CurrentLayer, CurrentLayer - 1);
                CurrentLayer--;
            }
        }
        ImGui::EndGroup();

        ImGui::End();
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

        RenderSceneHierarchy();

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        RenderObjectProperties();

        if (bIsFloating) ImGui::End();
        else ImGui::EndChild();
    }

    void LevelDesignerScene::RenderSceneHierarchy()
    {
        ImGui::SeparatorText("Scene Hierarchy");

        float HierarchyHeight = ImGui::GetContentRegionAvail().y * 0.4f;
        ImGui::BeginChild("HierarchyList", ImVec2(0, HierarchyHeight), true);

        const std::vector<std::shared_ptr<Core::WorldObject>>& AllObjects = World.GetAllObjects();

        std::shared_ptr<Core::WorldObject> SelectedPtr = SelectedObject.lock();

        for (size_t i = 0; i < AllObjects.size(); ++i)
        {
            const std::shared_ptr<Core::WorldObject>& Obj = AllObjects[i];
            if (!Obj)
                continue;

            if (Obj->GetTag() == Core::ObjectTag::Editor)
                continue;

            bool bIsSelected = (SelectedPtr == Obj);

            std::string ObjectName = Obj->GetName().empty() ? "[Unnamed]" : Obj->GetName();

            ImGui::PushID(static_cast<int>(i));

            float ButtonWidth = 20.0f;
            float AvailableWidth = ImGui::GetContentRegionAvail().x;

            if (ImGui::Selectable(ObjectName.c_str(), bIsSelected, 0, ImVec2(AvailableWidth - ButtonWidth * 2 - 8, 0)))
            {
                SelectedObject = Obj;
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("^"))
            {
                World.MoveObjectUp(Obj);
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("v"))
            {
                World.MoveObjectDown(Obj);
            }

            ImGui::PopID();
        }

        if (ImGui::BeginPopupContextWindow("HierarchyContextMenu"))
        {
            if (ImGui::MenuItem("Create Object"))
            {
                std::shared_ptr<Core::WorldObject> NewObject = World.CreateObject();
                NewObject->SetTag(Core::ObjectTag::Game);
                NewObject->SetName("New Object");
                NewObject->Components().Add<Core::TransformComponent>();
                SelectedObject = NewObject;
            }

            ImGui::EndPopup();
        }

        ImGui::EndChild();
    }

    void LevelDesignerScene::RenderObjectProperties()
    {
        ImGui::SeparatorText("Properties");

        std::shared_ptr<Core::WorldObject> SelectedPtr = SelectedObject.lock();

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

        std::vector<Core::Component*> ComponentsToRemove;

        {
            const auto& AllComponents = SelectedPtr->Components().GetAll();

            for (const auto& ComponentPair : AllComponents)
            {
                Core::Component* Comp = ComponentPair.second.get();
                if (!Comp)
                    continue;

                Core::IComponentEditor* Editor = Core::ComponentEditorRegistry::Get().GetEditor(Comp);
                if (Editor)
                {
                    Editor->OnRemoveRequested = [&ComponentsToRemove](Core::Component* CompToRemove)
                    {
                        ComponentsToRemove.push_back(CompToRemove);
                    };

                    Editor->RenderEditor(Comp);

                    Editor->OnRemoveRequested = nullptr;
                }
                else
                {
                    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.0f, 1.0f), "No editor for %s", Comp->GetName().c_str());
                }

                ImGui::Spacing();
            }
        }

        for (Core::Component* CompToRemove : ComponentsToRemove)
        {
            SelectedPtr->Components().Remove(CompToRemove);
        }

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Add Component", ImVec2(-1, 0)))
        {
            bShowAddComponentModal = true;
            SelectedComponentTypeIndex = 0;
        }

        if (bShowAddComponentModal)
        {
            ImGui::OpenPopup("Add Component");
            bShowAddComponentModal = false;
        }

        if (ImGui::BeginPopupModal("Add Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            std::shared_ptr<Core::WorldObject> ModalSelectedPtr = SelectedObject.lock();

            if (!ModalSelectedPtr)
            {
                ImGui::Text("No object selected");
                if (ImGui::Button("Close", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
            else
            {
                std::vector<std::string> AllTypes = Core::ComponentRegistry::Get().GetRegisteredTypeNames();

                const auto& CurrentComponents = ModalSelectedPtr->Components().GetAll();

                std::vector<std::string> AvailableTypes;
                for (const std::string& TypeName : AllTypes)
                {
                    bool bAlreadyAttached = false;
                    for (const auto& ComponentPair : CurrentComponents)
                    {
                        if (ComponentPair.second && ComponentPair.second->GetName() == TypeName)
                        {
                            bAlreadyAttached = true;
                            break;
                        }
                    }

                    if (!bAlreadyAttached)
                    {
                        AvailableTypes.push_back(TypeName);
                    }
                }

                if (AvailableTypes.empty())
                {
                    ImGui::Text("No components available to add");
                }
                else
                {
                    if (SelectedComponentTypeIndex >= static_cast<int>(AvailableTypes.size()))
                    {
                        SelectedComponentTypeIndex = 0;
                    }

                    ImGui::Text("Component Type");
                    if (ImGui::BeginCombo("##ComponentType", AvailableTypes[SelectedComponentTypeIndex].c_str()))
                    {
                        for (int i = 0; i < static_cast<int>(AvailableTypes.size()); ++i)
                        {
                            bool bIsSelected = (SelectedComponentTypeIndex == i);
                            if (ImGui::Selectable(AvailableTypes[i].c_str(), bIsSelected))
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
                        std::shared_ptr<Core::Component> NewComponent = Core::ComponentRegistry::Get().Create(
                            AvailableTypes[SelectedComponentTypeIndex],
                            ModalSelectedPtr,
                            Context
                        );

                        if (NewComponent)
                        {
                            ModalSelectedPtr->Components().Attach(NewComponent);
                        }

                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();
                }

                if (ImGui::Button("Cancel", ImVec2(120, 0)))
                {
                    ImGui::CloseCurrentPopup();
                }
            }

            ImGui::EndPopup();
        }
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

    std::shared_ptr<Core::TileMapComponent> LevelDesignerScene::GetSelectedTileMap() const
    {
        std::shared_ptr<Core::WorldObject> SelectedPtr = SelectedObject.lock();
        if (!SelectedPtr)
        {
            return nullptr;
        }

        return SelectedPtr->Components().Get<Core::TileMapComponent>();
    }

    sf::FloatRect LevelDesignerScene::CalculateCanvasRect() const
    {
        float MenuBarHeight = ImGui::GetFrameHeight();

        bool bHasTileMap = GetSelectedTileMap() != nullptr;

        float CanvasX = (bTilePaletteFloating || !bHasTileMap) ? 0.0f : TilePalettePanelWidth + 4.0f;
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

    void LevelDesignerScene::SaveScene()
    {
        if (!CurrentScene.IsValid())
        {
            bShowSaveAsModal = true;
        }
        else
        {
            nlohmann::json SceneJson = World.ToJson();

            std::ofstream File(CurrentScene.GetPath());
            if (File.is_open())
            {
                File << SceneJson.dump(4);
                File.close();
            }
        }
    }

    void LevelDesignerScene::SaveSceneAs(const std::string& SceneName)
    {
        std::string FilePath = "Game/Assets/Scenes/" + SceneName + ".json";

        nlohmann::json SceneJson = World.ToJson();

        std::ofstream File(FilePath);
        if (File.is_open())
        {
            File << SceneJson.dump(4);
            File.close();

            CurrentScene.SetPath(FilePath);
            memset(SaveAsSceneNameBuffer, 0, sizeof(SaveAsSceneNameBuffer));
        }
    }

    void LevelDesignerScene::ZoomIn()
    {
        std::shared_ptr<Core::WorldObject> CameraObject = World.GetObjectByName("EditorCamera");
        if (!CameraObject)
            return;

        std::shared_ptr<Core::CameraComponent> Camera = CameraObject->Components().Get<Core::CameraComponent>();
        if (!Camera)
            return;

        float CurrentZoom = Camera->GetZoom();
        float NewZoom = CurrentZoom * 0.9f;
        NewZoom = std::clamp(NewZoom, 0.25f, 4.0f);
        Camera->SetZoom(NewZoom);
    }

    void LevelDesignerScene::ZoomOut()
    {
        std::shared_ptr<Core::WorldObject> CameraObject = World.GetObjectByName("EditorCamera");
        if (!CameraObject)
            return;

        std::shared_ptr<Core::CameraComponent> Camera = CameraObject->Components().Get<Core::CameraComponent>();
        if (!Camera)
            return;

        float CurrentZoom = Camera->GetZoom();
        float NewZoom = CurrentZoom * 1.1f;
        NewZoom = std::clamp(NewZoom, 0.25f, 4.0f);
        Camera->SetZoom(NewZoom);
    }

    void LevelDesignerScene::ResetView()
    {
        std::shared_ptr<Core::WorldObject> CameraObject = World.GetObjectByName("EditorCamera");
        if (!CameraObject)
            return;

        std::shared_ptr<Core::CameraComponent> Camera = CameraObject->Components().Get<Core::CameraComponent>();
        if (!Camera)
            return;

        Camera->SetZoom(0.25f);
        if (CameraObject->Transform())
        {
            CameraObject->Transform()->Position = sf::Vector2f(0.0f, 0.0f);
        }
    }

    void LevelDesignerScene::DrawSceneGrid()
    {
        if (!bShowGrid)
            return;

        std::shared_ptr<Core::WorldObject> CameraObject = World.GetObjectByName("EditorCamera");
        if (!CameraObject)
            return;

        std::shared_ptr<Core::CameraComponent> Camera = CameraObject->Components().Get<Core::CameraComponent>();
        if (!Camera)
            return;

        sf::View View = Camera->GetView();
        sf::Vector2f ViewCenter = View.getCenter();
        sf::Vector2f ViewSize = View.getSize();

        float Left = ViewCenter.x - ViewSize.x / 2.0f;
        float Right = ViewCenter.x + ViewSize.x / 2.0f;
        float Top = ViewCenter.y - ViewSize.y / 2.0f;
        float Bottom = ViewCenter.y + ViewSize.y / 2.0f;

        const float GridSize = 16.0f;

        int StartX = static_cast<int>(std::floor(Left / GridSize));
        int EndX = static_cast<int>(std::ceil(Right / GridSize));
        int StartY = static_cast<int>(std::floor(Top / GridSize));
        int EndY = static_cast<int>(std::ceil(Bottom / GridSize));

        sf::VertexArray Lines(sf::PrimitiveType::Lines);
        sf::Color GridColor(200, 200, 200, 80);

        for (int x = StartX; x <= EndX; ++x)
        {
            if (x == 0)
                continue;
            float XPos = x * GridSize;
            Lines.append(sf::Vertex(sf::Vector2f(XPos, Top), GridColor));
            Lines.append(sf::Vertex(sf::Vector2f(XPos, Bottom), GridColor));
        }

        for (int y = StartY; y <= EndY; ++y)
        {
            if (y == 0)
                continue;
            float YPos = y * GridSize;
            Lines.append(sf::Vertex(sf::Vector2f(Left, YPos), GridColor));
            Lines.append(sf::Vertex(sf::Vector2f(Right, YPos), GridColor));
        }

        Context->Window->draw(Lines);

        sf::Color OriginColor(255, 255, 255, 150);
        const float OriginThickness = 2.0f;

        sf::VertexArray VerticalOrigin(sf::PrimitiveType::Triangles);
        VerticalOrigin.append(sf::Vertex(sf::Vector2f(-OriginThickness / 2.0f, Top), OriginColor));
        VerticalOrigin.append(sf::Vertex(sf::Vector2f(OriginThickness / 2.0f, Top), OriginColor));
        VerticalOrigin.append(sf::Vertex(sf::Vector2f(OriginThickness / 2.0f, Bottom), OriginColor));
        VerticalOrigin.append(sf::Vertex(sf::Vector2f(-OriginThickness / 2.0f, Top), OriginColor));
        VerticalOrigin.append(sf::Vertex(sf::Vector2f(OriginThickness / 2.0f, Bottom), OriginColor));
        VerticalOrigin.append(sf::Vertex(sf::Vector2f(-OriginThickness / 2.0f, Bottom), OriginColor));

        sf::VertexArray HorizontalOrigin(sf::PrimitiveType::Triangles);
        HorizontalOrigin.append(sf::Vertex(sf::Vector2f(Left, -OriginThickness / 2.0f), OriginColor));
        HorizontalOrigin.append(sf::Vertex(sf::Vector2f(Right, -OriginThickness / 2.0f), OriginColor));
        HorizontalOrigin.append(sf::Vertex(sf::Vector2f(Right, OriginThickness / 2.0f), OriginColor));
        HorizontalOrigin.append(sf::Vertex(sf::Vector2f(Left, -OriginThickness / 2.0f), OriginColor));
        HorizontalOrigin.append(sf::Vertex(sf::Vector2f(Right, OriginThickness / 2.0f), OriginColor));
        HorizontalOrigin.append(sf::Vertex(sf::Vector2f(Left, OriginThickness / 2.0f), OriginColor));

        Context->Window->draw(VerticalOrigin);
        Context->Window->draw(HorizontalOrigin);
    }
}
