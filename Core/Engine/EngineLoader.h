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
        void LoadGlobalTileSheets();
        void CreateTileSheetObjects();

        std::shared_ptr<EngineContext> Context;
    };
}
