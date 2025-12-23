#pragma once

#include <memory>
#include <typeindex>
#include <unordered_map>
#include "IComponentEditor.h"

namespace Core
{
    class Component;

    class ComponentEditorRegistry
    {
    public:
        static ComponentEditorRegistry& Get();

        template <typename TComponent, typename TEditor>
        void Register()
        {
            std::type_index TypeIndex(typeid(TComponent));
            Editors[TypeIndex] = std::make_unique<TEditor>();
        }

        IComponentEditor* GetEditor(Component* InComponent);

    private:
        ComponentEditorRegistry() = default;
        std::unordered_map<std::type_index, std::unique_ptr<IComponentEditor>> Editors;
    };

#define REGISTER_COMPONENT_EDITOR(ComponentType, EditorType) \
    namespace { \
        static bool ComponentType##_editor_registered = []() { \
            Core::ComponentEditorRegistry::Get().Register<ComponentType, EditorType>(); \
            return true; \
        }(); \
    }
}
