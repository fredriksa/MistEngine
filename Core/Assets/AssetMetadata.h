#pragma once

#include <string>

namespace Core
{
	enum class AssetType
	{
		INVALID,
		Texture,
		Font,
		Sound
	};

	struct AssetMetadata
	{
		AssetType Type;
		
		// Original file path (for debugging/logging)
		std::string Path; 
	};
}
