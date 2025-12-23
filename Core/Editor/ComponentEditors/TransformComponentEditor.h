#pragma once

#include "../IComponentEditor.h"

namespace Core
{
    class TransformComponentEditor : public IComponentEditor
    {
    public:
        void RenderEditor(Component* InComponent) override;
    };
}
