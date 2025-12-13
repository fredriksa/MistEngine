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
        const size_t LastSlash = Path.find_last_of("/\\");
        const size_t LastDot = Path.find_last_of('.');
        Asset.Name = Path.substr(LastSlash + 1, LastDot - LastSlash - 1);
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
