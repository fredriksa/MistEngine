#pragma once

#include "CoreSystem.hpp"

namespace Core
{
    class ImGuiSystem;

    class InputSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::InputSystem;

        InputSystem(std::shared_ptr<EngineContext> InContext);

        void Start() override;
        void Tick(float DeltaTimeS) override;

    private:
        std::shared_ptr<ImGuiSystem> ImGuiSystemPtr;
    };
}
