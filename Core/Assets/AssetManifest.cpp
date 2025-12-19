#include "AssetManifest.h"

#include <fstream>
#include <cstdio>

#include "../ThirdParty/json.hpp"

using json = nlohmann::json;

namespace Core
{
    AssetManifest AssetManifest::LoadFromFile(const std::string& path, const std::string& basePath)
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
            json Data = json::parse(File);

            constexpr const char* FontsKey = "fonts";
            if (Data.contains(FontsKey))
            {
                for (const auto& FontJson : Data[FontsKey])
                {
                    AssetEntry Entry;
                    Entry.Id = FontJson.value("id", "");
                    Entry.Path = basePath + FontJson.value("path", "");
                    Entry.Size = FontJson.value("size", 16);

                    if (!Entry.Path.empty())
                    {
                        Manifest.Fonts.push_back(Entry);
                    }
                }
            }

            constexpr const char* TexturesKey = "textures";
            if (Data.contains(TexturesKey))
            {
                for (const auto& TextureJson : Data[TexturesKey])
                {
                    AssetEntry Entry;
                    Entry.Id = TextureJson.value("id", "");
                    Entry.Path = basePath + TextureJson.value("path", "");

                    if (!Entry.Path.empty())
                    {
                        Manifest.Textures.push_back(Entry);
                    }
                }
            }

            constexpr const char* SoundsKey = "sounds";
            if (Data.contains(SoundsKey))
            {
                for (const auto& SoundJson : Data[SoundsKey])
                {
                    AssetEntry Entry;
                    Entry.Id = SoundJson.value("id", "");
                    Entry.Path = basePath + SoundJson.value("path", "");

                    if (!Entry.Path.empty())
                    {
                        Manifest.Sounds.push_back(Entry);
                    }
                }
            }

            constexpr const char* ObjectsKey = "objects";
            if (Data.contains(ObjectsKey))
            {
                for (const auto& ObjectJson : Data[ObjectsKey])
                {
                    Manifest.Objects.push_back({
                        .Type = ObjectJson.value("type", ""),
                        .Overrides = ObjectJson
                    });
                }
            }

            std::printf("Loaded asset manifest: %s (%zu fonts, %zu textures, %zu sounds, %zu objects)\n",
                        path.c_str(), Manifest.Fonts.size(), Manifest.Textures.size(), Manifest.Sounds.size(),
                        Manifest.Objects.size());
        }
        catch (const json::exception& E)
        {
            std::printf("Failed to parse asset manifest %s: %s\n", path.c_str(), E.what());
        }

        return Manifest;
    }
}
