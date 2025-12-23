#pragma once

#include <memory>
#include <functional>

namespace Core
{
    class Component;

    class IComponentEditor
    {
    public:
        virtual ~IComponentEditor() = default;

        virtual void RenderEditor(Component* InComponent) = 0;

        std::function<void(Component*)> OnRemoveRequested;
    };
}
