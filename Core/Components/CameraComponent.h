#pragma once

#include "Component.h"
#include <SFML/Graphics/View.hpp>

namespace Core
{
    class CameraComponent : public Component
    {
    public:
        CameraComponent(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context);

        void SetZoom(float Zoom);
        void SetViewport(float Left, float Top, float Width, float Height);

        float GetZoom() const { return ZoomLevel; }
        sf::FloatRect GetViewport() const { return Viewport; }

        sf::View GetView() const;

    private:
        float ZoomLevel = 1.0f;
        sf::FloatRect Viewport{{0.0f, 0.0f}, {1.0f, 1.0f}};
        sf::Vector2f BaseSize;
    };
}
