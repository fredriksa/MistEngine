#pragma once

#include "CoreSystem.hpp"
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <functional>
#include <vector>

namespace Core
{
    class ShaderPipeline : public CoreSystem
    {
    public:
        static constexpr ECoreSystemType Type = ECoreSystemType::ShaderPipeline;

        struct ShaderPass
        {
            sf::Shader Shader;
            std::function<void(sf::Shader&)> SetUniforms;
            bool bEnabled = true;
        };

        ShaderPipeline(std::shared_ptr<EngineContext> Context);

        void Start() override;

        void ApplyAll(sf::RenderTexture& InputTexture, sf::RenderTexture& OutputTexture);

        void AddPass(ShaderPass&& Pass);
        void EnablePass(int Index, bool bEnable);

    private:
        std::vector<ShaderPass> Passes;
    };
}
