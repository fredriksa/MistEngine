#include "ComponentEditorBase.h"
#include "imgui.h"

namespace Core
{
    void ComponentEditorBase::RenderEditor(Component* InComponent)
    {
        std::string ComponentName = GetComponentName();
        std::string TreeNodeId = ComponentName + "##TreeNode";

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

        float ButtonWidth = 20.0f;
        float AvailableWidth = ImGui::GetContentRegionAvail().x;

        ImGui::PushItemWidth(AvailableWidth - ButtonWidth - 4.0f);

        ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_DefaultOpen |
                                   ImGuiTreeNodeFlags_AllowItemOverlap |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

        bool bOpen = ImGui::TreeNodeEx(TreeNodeId.c_str(), Flags, "%s", ComponentName.c_str());

        ImGui::PopItemWidth();

        ImGui::SameLine(AvailableWidth - ButtonWidth);
        std::string ButtonId = "X##Remove" + ComponentName;
        if (ImGui::SmallButton(ButtonId.c_str()))
        {
            if (OnRemoveRequested)
            {
                OnRemoveRequested(InComponent);
            }
        }

        ImGui::PopStyleVar();

        if (bOpen)
        {
            RenderContent(InComponent);
            ImGui::TreePop();
        }
    }
}
