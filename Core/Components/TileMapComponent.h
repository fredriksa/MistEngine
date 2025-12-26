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

		uint GetWidth() const { return TileMapData.GetWidth(); }
		uint GetHeight() const { return TileMapData.GetHeight(); }
		uint GetLayerCount() const { return TileMapData.GetLayerCount(); }
		const Tile& GetTile(uint X, uint Y, uint Layer) const { return TileMapData.GetTile(X, Y, Layer); }

		struct TileBounds
		{
			uint MinX = 0;
			uint MinY = 0;
			uint MaxX = 0;
			uint MaxY = 0;
			bool IsValid = false;
		};

		TileBounds GetValidTileBounds() const;

		nlohmann::json ToJson() const;

	private:
		void UpdateLayerVisibility();

		TileMap TileMapData{DefaultTileMapSize, DefaultTileMapSize};
		std::vector<bool> LayerVisibility;
	};
}
