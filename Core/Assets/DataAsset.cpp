#include "DataAsset.h"

#include <fstream>

namespace Core
{
    std::optional<DataAsset> DataAsset::LoadFromFile(const std::string& Path)
    {
        std::ifstream File(Path);
        if (!File.is_open())
        {
            std::printf("Failed to open DataAsset: %s\n", Path.c_str());
            return std::nullopt;
        }

        nlohmann::json Json;
        File >> Json;

        DataAsset Asset;
        Asset.Name = Json.value("name", "Unknown");
        if (Json.contains("components") && Json["components"].is_array())
        {
            for (const auto& ComponentJson : Json["components"])
            {
                ComponentData CompData;
                CompData.Type = ComponentJson.value("type", "");
                CompData.Data = ComponentJson["data"];
                Asset.Components.push_back(CompData);
            }
        }

        return Asset;
    }
}
