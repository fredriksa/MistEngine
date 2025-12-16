#pragma once

namespace Core
{
    class IEngine
    {
    public:
        virtual ~IEngine() = default;
        virtual void Shutdown() = 0;
    };
}

