#include "TileMapComponentEditor.h"
#include "../../Components/TileMapComponent.h"
#include "../ComponentEditorRegistry.h"
#include "imgui.h"

namespace Core
{
    REGISTER_COMPONENT_EDITOR(TileMapComponent, TileMapComponentEditor);

    std::string TileMapComponentEditor::GetComponentName() const
    {
        return "TileMap Component";
    }

    void TileMapComponentEditor::RenderContent(Component* InComponent)
    {
        TileMapComponent* TileMapComp = dynamic_cast<TileMapComponent*>(InComponent);
        if (!TileMapComp)
        {
            return;
        }

        TileMap& TileMapData = TileMapComp->GetTileMap();

        int Width = static_cast<int>(TileMapData.GetWidth());
        int Height = static_cast<int>(TileMapData.GetHeight());

        if (ImGui::InputInt("Width", &Width))
        {
            if (Width >= static_cast<int>(TileMapComponent::MinTileMapSize) &&
                Width <= static_cast<int>(TileMapComponent::MaxTileMapSize))
            {
                TileMapData.Resize(static_cast<Core::uint>(Width), TileMapData.GetHeight());
            }
        }

        if (ImGui::InputInt("Height", &Height))
        {
            if (Height >= static_cast<int>(TileMapComponent::MinTileMapSize) &&
                Height <= static_cast<int>(TileMapComponent::MaxTileMapSize))
            {
                TileMapData.Resize(TileMapData.GetWidth(), static_cast<Core::uint>(Height));
            }
        }

        ImGui::Text("Layers: %u", TileMapData.GetLayerCount());
    }
}
