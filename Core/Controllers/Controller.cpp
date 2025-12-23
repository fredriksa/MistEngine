#include "Controller.h"

#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/InputSystem.h"

namespace Core
{
    Controller::Controller(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context, InputSource Source, const std::string& TypeName)
        : Component(Owner, std::move(Context), TypeName)
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

    void Controller::Shutdown()
    {
        if (std::shared_ptr<InputSystem> Input = GetContext().SystemsRegistry->GetCoreSystem<InputSystem>())
        {
            Input->UnregisterController(this);
        }
    }
}
