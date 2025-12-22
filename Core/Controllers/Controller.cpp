#include "Controller.h"

#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/InputSystem.h"

namespace Core
{
    Controller::Controller(const std::shared_ptr<WorldObject>& Owner, InputSource Source)
        : Component(Owner)
          , AssignedSource(Source)
    {
        if (std::shared_ptr<InputSystem> Input = GetContext().SystemsRegistry->GetCoreSystem<InputSystem>())
        {
            Input->RegisterController(this);
        }
    }

    Controller::~Controller()
    {
        if (std::shared_ptr<InputSystem> Input = GetContext().SystemsRegistry->GetCoreSystem<InputSystem>())
        {
            Input->UnregisterController(this);
        }
    }
}
