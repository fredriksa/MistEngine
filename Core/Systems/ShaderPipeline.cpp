#include "ShaderPipeline.h"
#include "SceneManagerSystem.h"
#include "../Scene/Scene.h"
#include "../World/World.h"
#include "../World/WorldEnvironment.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"

namespace Core
{
    ShaderPipeline::ShaderPipeline(std::shared_ptr<EngineContext> Context)
        : CoreSystem("ShaderPipeline", Type, std::move(Context))
    {
    }

    void ShaderPipeline::Start()
    {
        ShaderPass DayNightPass;

        if (!DayNightPass.Shader.loadFromFile("Core/Shaders/daynight.frag", sf::Shader::Type::Fragment))
        {
            std::printf("Failed to load daynight.frag shader\n");
            return;
        }

        DayNightPass.SetUniforms = [this](sf::Shader& Shader)
        {
            std::shared_ptr<SceneManagerSystem> SceneManager =
                GetContext()->SystemsRegistry->GetCoreSystem<SceneManagerSystem>();

            if (SceneManager)
            {
                std::shared_ptr<Scene> ActiveScene = SceneManager->GetActiveScene();
                if (ActiveScene)
                {
                    const WorldEnvironment& Env = ActiveScene->GetWorld().GetEnvironment();
                    const float CurrentTime = Env.GetTime().GetHours();
                    const DayNightColors& Colors = Env.GetDayNightColors();

                    Shader.setUniform("time", CurrentTime);
                    Shader.setUniform("nightColor", sf::Glsl::Vec3(Colors.Night));
                    Shader.setUniform("sunriseColor", sf::Glsl::Vec3(Colors.Sunrise));
                    Shader.setUniform("dayColor", sf::Glsl::Vec3(Colors.Day));
                    Shader.setUniform("sunsetColor", sf::Glsl::Vec3(Colors.Sunset));
                }
            }
        };

        AddPass(std::move(DayNightPass));
    }

    void ShaderPipeline::ApplyAll(sf::RenderTexture& InputTexture, sf::RenderTexture& OutputTexture)
    {
        if (Passes.empty())
        {
            return;
        }

        sf::RenderTexture TempTexture(InputTexture.getSize());

        sf::RenderTexture* CurrentInput = &InputTexture;
        sf::RenderTexture* CurrentOutput = &TempTexture;

        for (size_t i = 0; i < Passes.size(); ++i)
        {
            ShaderPass& Pass = Passes[i];

            if (!Pass.bEnabled)
                continue;

            if (i == Passes.size() - 1)
            {
                CurrentOutput = &OutputTexture;
            }

            Pass.SetUniforms(Pass.Shader);

            CurrentOutput->clear();
            sf::Sprite Sprite(CurrentInput->getTexture());
            CurrentOutput->draw(Sprite, &Pass.Shader);
            CurrentOutput->display();

            if (i < Passes.size() - 1)
            {
                std::swap(CurrentInput, CurrentOutput);
            }
        }
    }

    void ShaderPipeline::AddPass(ShaderPass&& Pass)
    {
        Passes.push_back(std::move(Pass));
    }

    void ShaderPipeline::EnablePass(int Index, bool bEnable)
    {
        if (Index >= 0 && Index < static_cast<int>(Passes.size()))
        {
            Passes[Index].bEnabled = bEnable;
        }
    }
}
