#include "TileMapComponent.h"

#include "ComponentRegistry.h"
#include "TransformComponent.h"
#include "../EngineContext.hpp"
#include "../SystemsRegistry.hpp"
#include "../Systems/AssetRegistrySystem.h"
#include "../TileMap/TileSheet.h"
#include "../World/WorldConstants.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace Core
{
    REGISTER_COMPONENT(TileMapComponent);

    TileMapComponent::TileMapComponent(const std::shared_ptr<WorldObject>& Owner,
                                       std::shared_ptr<EngineContext> Context)
        : Component(Owner, std::move(Context), "TileMapComponent")
    {
    }

    bool TileMapComponent::Initialize(const nlohmann::json& Data)
    {
        uint Width = Data.value("width", 0u);
        uint Height = Data.value("height", 0u);

        if (Width == 0 || Height == 0)
        {
            return false;
        }

        TileMapData = TileMap(Width, Height);

        if (Data.contains("layers") && Data["layers"].is_array())
        {
            const nlohmann::json& LayersArray = Data["layers"];

            for (size_t LayerIndex = 1; LayerIndex < LayersArray.size(); ++LayerIndex)
            {
                TileMapData.AddLayer();
            }

            for (size_t LayerIndex = 0; LayerIndex < LayersArray.size(); ++LayerIndex)
            {
                const nlohmann::json& LayerData = LayersArray[LayerIndex];

                if (LayerData.contains("tiles") && LayerData["tiles"].is_array())
                {
                    const nlohmann::json& TilesArray = LayerData["tiles"];

                    for (const auto& TileJson : TilesArray)
                    {
                        uint X = TileJson.value("x", 0u);
                        uint Y = TileJson.value("y", 0u);
                        Tile LoadedTile = Tile::FromJson(TileJson);
                        TileMapData.SetTile(X, Y, static_cast<uint>(LayerIndex), LoadedTile);
                    }
                }
            }
        }
        else if (Data.contains("tiles") && Data["tiles"].is_array())
        {
            const nlohmann::json& TilesArray = Data["tiles"];

            for (const auto& TileJson : TilesArray)
            {
                uint X = TileJson.value("x", 0u);
                uint Y = TileJson.value("y", 0u);
                Tile LoadedTile = Tile::FromJson(TileJson);
                TileMapData.SetTile(X, Y, 0, LoadedTile);
            }
        }

        UpdateLayerVisibility();
        return true;
    }

    void TileMapComponent::SetSize(uint Width, uint Height)
    {
        TileMapData = TileMap(Width, Height);
        UpdateLayerVisibility();
    }

    void TileMapComponent::UpdateLayerVisibility()
    {
        uint LayerCount = TileMapData.GetLayerCount();
        if (LayerVisibility.size() != LayerCount)
        {
            LayerVisibility.resize(LayerCount, true);
        }
    }

    void TileMapComponent::SetLayerVisible(uint Layer, bool bVisible)
    {
        UpdateLayerVisibility();
        if (Layer < LayerVisibility.size())
        {
            LayerVisibility[Layer] = bVisible;
        }
    }

    bool TileMapComponent::IsLayerVisible(uint Layer) const
    {
        if (Layer < LayerVisibility.size())
        {
            return LayerVisibility[Layer];
        }
        return true;
    }

    void TileMapComponent::Render()
    {
        const EngineContext& Context = GetContext();
        std::shared_ptr<AssetRegistrySystem> AssetRegistry = Context.SystemsRegistry->GetCoreSystem<
            AssetRegistrySystem>();
        if (!AssetRegistry)
        {
            return;
        }

        std::vector<std::shared_ptr<const TileSheet>> TileSheets = AssetRegistry->GetAllTileSheets();
        if (TileSheets.empty())
        {
            return;
        }

        sf::Vector2f ObjectPosition(0.0f, 0.0f);
        if (WorldObject* Owner = GetOwner())
        {
            if (std::shared_ptr<TransformComponent> Transform = Owner->Components().Get<TransformComponent>())
            {
                ObjectPosition = Transform->Position;
            }
        }

        for (uint Layer = 0; Layer < TileMapData.GetLayerCount(); ++Layer)
        {
            if (!IsLayerVisible(Layer))
            {
                continue;
            }

            for (uint Y = 0; Y < TileMapData.GetHeight(); ++Y)
            {
                for (uint X = 0; X < TileMapData.GetWidth(); ++X)
                {
                    const Tile& Tile = TileMapData.GetTile(X, Y, Layer);

                    if (Tile.IsEmpty())
                    {
                        continue;
                    }

                    std::optional<uint> TileSheetId = Tile.GetTileSheetId();
                    uint TileSheetIndex = TileSheetId.value();

                    if (TileSheetIndex >= TileSheets.size())
                    {
                        continue;
                    }

                    std::shared_ptr<const TileSheet> TileSheet = TileSheets[TileSheetIndex];
                    if (!TileSheet)
                    {
                        continue;
                    }

                    std::shared_ptr<const sf::Texture> Texture = TileSheet->GetTexture();
                    if (!Texture)
                    {
                        continue;
                    }

                    sf::IntRect TileRect = TileSheet->GetTileRect(Tile.GetTileIndex());

                    sf::Sprite TileSprite(*Texture);
                    TileSprite.setTextureRect(TileRect);

                    sf::Vector2f TileLocalPos(X * WorldConstants::TileSize, Y * WorldConstants::TileSize);
                    TileSprite.setPosition(TileLocalPos + ObjectPosition);

                    Context.Window->draw(TileSprite);
                }
            }
        }
    }

    nlohmann::json TileMapComponent::ToJson() const
    {
        nlohmann::json LayersArray = nlohmann::json::array();

        for (uint Layer = 0; Layer < TileMapData.GetLayerCount(); ++Layer)
        {
            nlohmann::json TilesArray = nlohmann::json::array();

            for (uint Y = 0; Y < TileMapData.GetHeight(); ++Y)
            {
                for (uint X = 0; X < TileMapData.GetWidth(); ++X)
                {
                    const Tile& Tile = TileMapData.GetTile(X, Y, Layer);

                    if (!Tile.IsEmpty())
                    {
                        nlohmann::json TileJson = Tile.ToJson();
                        TileJson["x"] = X;
                        TileJson["y"] = Y;
                        TilesArray.push_back(TileJson);
                    }
                }
            }

            nlohmann::json LayerData;
            LayerData["tiles"] = TilesArray;
            LayersArray.push_back(LayerData);
        }

        return {
            {"width", TileMapData.GetWidth()},
            {"height", TileMapData.GetHeight()},
            {"layers", LayersArray}
        };
    }
}
