#include "ComponentEditorRegistry.h"
#include "../Components/Component.h"
#include <typeindex>

namespace Core
{
    ComponentEditorRegistry& ComponentEditorRegistry::Get()
    {
        static ComponentEditorRegistry Instance;
        return Instance;
    }

    IComponentEditor* ComponentEditorRegistry::GetEditor(Component* InComponent)
    {
        if (!InComponent)
        {
            return nullptr;
        }

        std::type_index TypeIndex(typeid(*InComponent));
        auto It = Editors.find(TypeIndex);

        if (It != Editors.end())
        {
            return It->second.get();
        }

        return nullptr;
    }
}
