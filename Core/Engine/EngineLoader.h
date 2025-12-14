#pragma once

#include <memory>

namespace Core
{
    struct EngineContext;

    class EngineLoader
    {
    public:
        EngineLoader(std::shared_ptr<EngineContext> Context);
        ~EngineLoader();

        void LoadGlobalAssets();

    private:
        std::shared_ptr<EngineContext> Context;
    };
}
