#pragma once

#include <memory>

namespace Core
{
    class Component;

    class IComponentEditor
    {
    public:
        virtual ~IComponentEditor() = default;

        virtual void RenderEditor(Component* InComponent) = 0;
    };
}
