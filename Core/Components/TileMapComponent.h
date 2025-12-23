#pragma once

#include "Component.h"
#include "ComponentRegistry.h"
#include "../TileMap/TileMap.h"

namespace Core
{
	class TileMapComponent : public Component
	{
	public:
		static constexpr uint MinTileMapSize = 1;
		static constexpr uint MaxTileMapSize = 1000;
		static constexpr uint DefaultTileMapSize = 100;

		TileMapComponent(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context);

		bool Initialize(const nlohmann::json& Data) override;
		void Render() override;

		TileMap& GetTileMap() { return TileMapData; }
		const TileMap& GetTileMap() const { return TileMapData; }

		void SetSize(uint Width, uint Height);

		void SetLayerVisible(uint Layer, bool bVisible);
		bool IsLayerVisible(uint Layer) const;

		nlohmann::json ToJson() const;

	private:
		void UpdateLayerVisibility();

		TileMap TileMapData{DefaultTileMapSize, DefaultTileMapSize};
		std::vector<bool> LayerVisibility;
	};
}
