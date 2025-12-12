#pragma once

#include <string>
#include <vector>

#include "../ThirdParty/json.hpp"

namespace Core
{
    struct AssetEntry
    {
        std::string Id;
        std::string Path;
        unsigned int Size = 16;
    };

    struct ObjectEntry
    {
        std::string Type;
        nlohmann::json Overrides;
    };

    struct AssetManifest
    {
        std::vector<AssetEntry> Fonts;
        std::vector<AssetEntry> Textures;
        std::vector<AssetEntry> Sounds;
        std::vector<ObjectEntry> Objects;

        static AssetManifest LoadFromFile(const std::string& path, const std::string& basePath = "");
    };
}
