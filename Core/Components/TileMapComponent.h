#pragma once

#include "Component.h"
#include "ComponentRegistry.h"
#include "../TileMap/TileMap.h"

namespace Core
{
	class TileMapComponent : public Component
	{
	public:
		TileMapComponent(const std::shared_ptr<WorldObject>& Owner, std::shared_ptr<EngineContext> Context);

		bool Initialize(const nlohmann::json& Data) override;
		void Render() override;

		TileMap& GetTileMap() { return TileMapData; }
		const TileMap& GetTileMap() const { return TileMapData; }

		nlohmann::json ToJson() const;

	private:
		TileMap TileMapData{0, 0};
	};
}
