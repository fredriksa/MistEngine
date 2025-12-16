#include "ImGuiSystem.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include "imgui-SFML.h"
#include "../EngineContext.hpp"

namespace Core
{
    ImGuiSystem::ImGuiSystem(std::shared_ptr<EngineContext> InContext)
        : CoreSystem("ImGuiSystem", Type, std::move(InContext))
    {
    }

    void ImGuiSystem::Start()
    {
        if (!ImGui::SFML::Init(*GetContext()->Window))
        {
            std::printf("Failed to initialize ImGui System\n");
        }
    }

    void ImGuiSystem::Tick(float DeltaTime)
    {
        ImGui::SFML::Update(*GetContext()->Window, sf::seconds(DeltaTime));
    }

    void ImGuiSystem::ProcessEvent(const sf::Event& Event)
    {
        ImGui::SFML::ProcessEvent(*GetContext()->Window, Event);
    }
    
    void ImGuiSystem::RenderUI()
    {
        ImGui::SFML::Render(*GetContext()->Window);
    }

    void ImGuiSystem::Shutdown()
    {
        ImGui::SFML::Shutdown();
    }
}
