#pragma once

#include "IComponentEditor.h"
#include <string>

namespace Core
{
    class ComponentEditorBase : public IComponentEditor
    {
    public:
        void RenderEditor(Component* InComponent) override final;

    protected:
        virtual std::string GetComponentName() const = 0;
        virtual void RenderContent(Component* InComponent) = 0;
    };
}
