#pragma once

#include "../ComponentEditorBase.h"

namespace Core
{
    class TransformComponentEditor : public ComponentEditorBase
    {
    protected:
        std::string GetComponentName() const override;
        void RenderContent(Component* InComponent) override;
    };
}
