#pragma once

#include "CoreSystem.hpp"

namespace sf
{
    class Event;
}

namespace Core
{
    class ImGuiSystem : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::ImGuiSystem;

        ImGuiSystem(std::shared_ptr<EngineContext> InContext);

        void Start() override;
        void Tick(float DeltaTime) override;
        void RenderUI() override;
        void Shutdown() override;
        
        void ProcessEvent(const sf::Event& event);
    };
}
