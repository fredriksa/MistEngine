#pragma once

#include "CoreSystem.hpp"
#include <vector>
#include <memory>

namespace Core
{
    class ImGuiSystem;
    class Controller;

    class InputSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::InputSystem;

        InputSystem(std::shared_ptr<EngineContext> InContext);

        void Start() override;
        void Tick(float DeltaTimeS) override;

        void RegisterController(Controller* InController);
        void UnregisterController(Controller* InController);

    private:
        std::shared_ptr<ImGuiSystem> ImGuiSystemPtr;
        std::vector<Controller*> Controllers;
    };
}
