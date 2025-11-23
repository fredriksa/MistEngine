#include "AssetManifest.h"

#include <fstream>
#include <cstdio>

#include "../ThirdParty/json.hpp"

using json = nlohmann::json;

namespace Core
{
    AssetManifest AssetManifest::LoadFromFile(const std::string& path)
    {
        AssetManifest Manifest;

        std::ifstream File(path);
        if (!File.is_open())
        {
            std::printf("Failed to open asset manifest: %s\n", path.c_str());
            return Manifest;
        }

        try
        {
            json J = json::parse(File);

            if (J.contains("fonts"))
            {
                for (const auto& FontJson : J["fonts"])
                {
                    AssetEntry Entry;
                    Entry.Id = FontJson.value("id", "");
                    Entry.Path = FontJson.value("path", "");
                    Entry.Size = FontJson.value("size", 16);

                    if (!Entry.Path.empty())
                    {
                        Manifest.Fonts.push_back(Entry);
                    }
                }
            }

            if (J.contains("textures"))
            {
                for (const auto& TextureJson : J["textures"])
                {
                    AssetEntry Entry;
                    Entry.Id = TextureJson.value("id", "");
                    Entry.Path = TextureJson.value("path", "");

                    if (!Entry.Path.empty())
                    {
                        Manifest.Textures.push_back(Entry);
                    }
                }
            }

            if (J.contains("sounds"))
            {
                for (const auto& SoundJson : J["sounds"])
                {
                    AssetEntry Entry;
                    Entry.Id = SoundJson.value("id", "");
                    Entry.Path = SoundJson.value("path", "");

                    if (!Entry.Path.empty())
                    {
                        Manifest.Sounds.push_back(Entry);
                    }
                }
            }

            std::printf("Loaded asset manifest: %s (%zu fonts, %zu textures, %zu sounds)\n",
                path.c_str(), Manifest.Fonts.size(), Manifest.Textures.size(), Manifest.Sounds.size());
        }
        catch (const json::exception& E)
        {
            std::printf("Failed to parse asset manifest %s: %s\n", path.c_str(), E.what());
        }

        return Manifest;
    }
}
