#pragma once

namespace Core
{
    class ITickable
    {
    public:
        virtual ~ITickable() = default;
        virtual void Tick(float DeltaTimeS) = 0;
    };
}
