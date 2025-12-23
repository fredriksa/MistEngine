#include "TransformComponentEditor.h"
#include "../../Components/TransformComponent.h"
#include "../ComponentEditorRegistry.h"
#include "imgui.h"

namespace Core
{
    REGISTER_COMPONENT_EDITOR(TransformComponent, TransformComponentEditor);

    std::string TransformComponentEditor::GetComponentName() const
    {
        return "Transform Component";
    }

    void TransformComponentEditor::RenderContent(Component* InComponent)
    {
        TransformComponent* Transform = dynamic_cast<TransformComponent*>(InComponent);
        if (!Transform)
        {
            return;
        }

        ImGui::Text("Position");
        ImGui::InputFloat("X##TransformPos", &Transform->Position.x);
        ImGui::InputFloat("Y##TransformPos", &Transform->Position.y);
    }
}
