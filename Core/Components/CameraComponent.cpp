#include "CameraComponent.h"

#include "ComponentRegistry.h"
#include "../EngineContext.hpp"
#include "../World/WorldObject.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include "TransformComponent.h"

namespace Core
{
    REGISTER_COMPONENT(CameraComponent);

    CameraComponent::CameraComponent(const std::shared_ptr<WorldObject>& Owner)
        : Component(Owner)
    {
        sf::Vector2u WindowSize = GetContext().Window->getSize();
        BaseSize = sf::Vector2f(static_cast<float>(WindowSize.x), static_cast<float>(WindowSize.y));
    }

    void CameraComponent::SetZoom(float Zoom)
    {
        ZoomLevel = Zoom;
    }

    void CameraComponent::SetViewport(float Left, float Top, float Width, float Height)
    {
        Viewport = sf::FloatRect{{Left, Top}, {Width, Height}};
    }

    sf::View CameraComponent::GetView() const
    {
        sf::View View;

        View.setCenter(GetOwner()->Transform()->Position);
        View.setSize(BaseSize * ZoomLevel);
        View.setViewport(Viewport);

        return View;
    }
}
