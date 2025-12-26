#include "LevelDesignerScene.h"
#include "../Components/CameraComponent.h"
#include "../Components/TileMapComponent.h"
#include "../Components/TransformComponent.h"
#include "../Controllers/LevelEditorController.h"
#include "../World/World.h"
#include "../World/WorldObject.h"
#include "../EngineContext.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/OpenGL.hpp>

namespace Core
{
    LevelDesignerScene::LevelDesignerScene(std::shared_ptr<EngineContext> InContext)
        : Scene(std::move(InContext), "LevelDesigner")
          , Model(World, Context)
          , ViewModel(Model)
          , View(ViewModel)
    {
    }

    void LevelDesignerScene::OnLoad()
    {
        Scene::OnLoad();
        CreateEditorInfrastructure();
    }

    void LevelDesignerScene::CreateEditorInfrastructure()
    {
        std::shared_ptr<WorldObject> CameraObj = Model.CreateObject();
        CameraObj->SetName("EditorCamera");
        CameraObj->SetTag(ObjectTag::Editor);
        CameraObj->Components().Add<TransformComponent>();
        CameraComponent* Camera = CameraObj->Components().Add<CameraComponent>();
        Camera->SetZoom(0.25f);
        CameraObj->Components().Add<LevelEditorController>();
    }

    void LevelDesignerScene::PreRender()
    {
        Scene::PreRender();

        View.CalculateCanvasRect();

        if (std::shared_ptr<WorldObject> CameraObject = Model.GetWorld().Objects().GetByName("EditorCamera"))
        {
            if (std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>())
            {
                Context->Window->setView(Camera->GetView());

                const sf::FloatRect CanvasRect = View.GetCanvasRect();
                const int ScissorX = static_cast<int>(CanvasRect.position.x);
                const int ScissorY = static_cast<int>(Context->WindowSize.y - CanvasRect.position.y - CanvasRect.size.
                    y);
                const int ScissorWidth = static_cast<int>(CanvasRect.size.x);
                const int ScissorHeight = static_cast<int>(CanvasRect.size.y);

                glEnable(GL_SCISSOR_TEST);
                glScissor(ScissorX, ScissorY, ScissorWidth, ScissorHeight);
            }
        }
    }

    void LevelDesignerScene::PostRender()
    {
        View.DrawSceneGrid();

        glDisable(GL_SCISSOR_TEST);

        Scene::PostRender();

        View.RenderMenuBar(LoadingTask);
        View.RenderMainContent();
    }

    bool LevelDesignerScene::IsClickInCanvas(WindowCoordinate MousePos) const
    {
        const sf::FloatRect CanvasRect = View.GetCanvasRect();
        return CanvasRect.contains(sf::Vector2f(static_cast<float>(MousePos.X()), static_cast<float>(MousePos.Y())));
    }

    void LevelDesignerScene::ZoomIn()
    {
        if (std::shared_ptr<WorldObject> CameraObject = Model.GetWorld().Objects().GetByName("EditorCamera"))
        {
            if (std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>())
            {
                const float CurrentZoom = Camera->GetZoom();
                Camera->SetZoom(CurrentZoom * 0.9f);
            }
        }
    }

    void LevelDesignerScene::ZoomOut()
    {
        if (std::shared_ptr<WorldObject> CameraObject = Model.GetWorld().Objects().GetByName("EditorCamera"))
        {
            if (std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>())
            {
                const float CurrentZoom = Camera->GetZoom();
                Camera->SetZoom(CurrentZoom * 1.1f);
            }
        }
    }

    void LevelDesignerScene::ResetView()
    {
        if (std::shared_ptr<WorldObject> CameraObject = Model.GetWorld().Objects().GetByName("EditorCamera"))
        {
            if (std::shared_ptr<CameraComponent> Camera = CameraObject->Components().Get<CameraComponent>())
            {
                Camera->SetZoom(0.25f);
            }

            if (std::shared_ptr<TransformComponent> Transform = CameraObject->Components().Get<TransformComponent>())
            {
                Transform->Position = sf::Vector2f(0.0f, 0.0f);
            }
        }
    }
}
