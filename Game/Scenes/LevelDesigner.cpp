#include "LevelDesigner.h"

#include "imgui.h"
#include "../../SystemsRegistry.hpp"
#include "../../Systems/SceneManagerSystem.h"

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

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Level", "Ctrl+N")) { /* TODO */ }
            if (ImGui::MenuItem("Open Level...", "Ctrl+O")) { /* TODO */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S")) { /* TODO */ }
            if (ImGui::MenuItem("Save As...")) { /* TODO */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit to Main Menu")) { ExitToMainMenu(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) { /* TODO */ }
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) { /* TODO */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear Level")) { /* TODO */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Show Grid", "G")) { /* TODO */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Zoom In", "+")) { /* TODO */ }
            if (ImGui::MenuItem("Zoom Out", "-")) { /* TODO */ }
            if (ImGui::MenuItem("Reset View", "R")) { /* TODO */ }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem("Select Tool", "V")) { /* TODO */ }
            if (ImGui::MenuItem("Paint Tool", "B")) { /* TODO */ }
            if (ImGui::MenuItem("Fill Tool", "F")) { /* TODO */ }
            if (ImGui::MenuItem("Eraser Tool", "E")) { /* TODO */ }
            ImGui::Separator();
            if (ImGui::MenuItem("Entity Placer")) { /* TODO */ }
            ImGui::EndMenu();
        }

        // Back button on far right
        float MenuBarHeight = ImGui::GetWindowSize().y;
        float BackButtonWidth = 80.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - BackButtonWidth);
        if (ImGui::Button("Back", ImVec2(BackButtonWidth - 10, MenuBarHeight - 4)))
        {
            ExitToMainMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // Get menu bar height for positioning
    float MenuBarHeight = ImGui::GetFrameHeight();

    // Main content area (palette + canvas + properties)
    ImGui::SetNextWindowPos(ImVec2(0, MenuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - MenuBarHeight));
    ImGui::Begin("LevelDesignerContent", nullptr,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoBringToFrontOnFocus);

    float ContentWidth = ImGui::GetContentRegionAvail().x;
    float ContentHeight = ImGui::GetContentRegionAvail().y;

    // Left: Tile Palette Panel
    RenderTilePalettePanel();

    ImGui::SameLine();

    // Middle: Canvas Area
    RenderCanvasArea();

    ImGui::SameLine();

    // Right: Properties Panel
    RenderPropertiesPanel();

    ImGui::End();
}

void Game::LevelDesignerScene::RenderTilePalettePanel()
{
    float ContentHeight = ImGui::GetContentRegionAvail().y;

    ImGui::BeginChild("TilePalette", ImVec2(TilePalettePanelWidth, ContentHeight), true);
    ImGui::SeparatorText("Tile Palette");

    // Tilesheet selection dropdown
    const char* TileSheets[] = { "terrain.tilesheet.png", "objects.tilesheet.png", "decorations.tilesheet.png" };
    ImGui::Text("Tilesheet:");
    ImGui::SetNextItemWidth(-1);
    ImGui::Combo("##TileSheetSelect", &SelectedTileSheetIndex, TileSheets, IM_ARRAYSIZE(TileSheets));

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Tile grid (scrollable)
    ImGui::BeginChild("TileGrid", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    // Placeholder: Show a grid of "tiles" (just colored boxes for preview)
    const int TilesPerRow = 8;
    const float TileDisplaySize = 24.0f;
    const float TilePadding = 4.0f;

    for (int i = 0; i < 64; ++i)  // 64 placeholder tiles
    {
        if (i % TilesPerRow != 0)
            ImGui::SameLine();

        ImGui::PushID(i);

        // Different colors for visual variety
        ImVec4 TileColor;
        if (i % 4 == 0) TileColor = ImVec4(0.3f, 0.6f, 0.3f, 1.0f);  // Green
        else if (i % 4 == 1) TileColor = ImVec4(0.6f, 0.4f, 0.2f, 1.0f);  // Brown
        else if (i % 4 == 2) TileColor = ImVec4(0.4f, 0.4f, 0.6f, 1.0f);  // Blue
        else TileColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // Gray

        // Highlight selected tile
        bool bIsSelected = (SelectedTileIndex == i);
        if (bIsSelected)
        {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));  // Yellow highlight
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Button, TileColor);
        }

        if (ImGui::Button("##Tile", ImVec2(TileDisplaySize, TileDisplaySize)))
        {
            SelectedTileIndex = i;
        }

        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    ImGui::EndChild();  // TileGrid
    ImGui::EndChild();  // TilePalette
}

void Game::LevelDesignerScene::RenderCanvasArea()
{
    float ContentHeight = ImGui::GetContentRegionAvail().y;
    float CanvasWidth = ImGui::GetContentRegionAvail().x - PropertiesPanelWidth - 4.0f;

    ImGui::BeginChild("Canvas", ImVec2(CanvasWidth, ContentHeight), true);
    ImGui::TextDisabled("Level Editor Canvas");
    ImGui::TextDisabled("(Tilemap will render here)");
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextDisabled("Controls:");
    ImGui::BulletText("Middle Mouse: Pan");
    ImGui::BulletText("Mouse Wheel: Zoom");
    ImGui::BulletText("Left Click: Paint tile");
    ImGui::BulletText("G: Toggle grid");
    ImGui::BulletText("B: Paint tool");
    ImGui::BulletText("E: Eraser tool");
    ImGui::EndChild();
}

void Game::LevelDesignerScene::RenderPropertiesPanel()
{
    float ContentHeight = ImGui::GetContentRegionAvail().y;

    // Resizable divider
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
    ImGui::SeparatorText("Properties");

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

    ImGui::EndChild();
}

void Game::LevelDesignerScene::ExitToMainMenu()
{
    Context->SystemsRegistry->GetCoreSystem<Core::SceneManagerSystem>()->Pop();
}
