#pragma once

#include "CoreSystem.hpp"

namespace Core
{
    class InputSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::InputSystem;

        InputSystem(std::shared_ptr<EngineContext> InContext);

        void Tick(float DeltaTimeS) override;
    };
}
