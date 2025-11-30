#pragma once

#include <string>
#include <vector>

namespace Core
{
    struct AssetEntry
    {
        std::string Id;
        std::string Path;
        unsigned int Size = 16;
    };

    struct AssetManifest
    {
        std::vector<AssetEntry> Fonts;
        std::vector<AssetEntry> Textures;
        std::vector<AssetEntry> Sounds;

        static AssetManifest LoadFromFile(const std::string& path, const std::string& basePath = "");
    };
}
