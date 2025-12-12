#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../ThirdParty/json.hpp"

namespace Core
{
    struct ComponentData
    {
        std::string Type;
        nlohmann::json Data;
    };

    struct DataAsset
    {
        std::string Name;
        std::vector<ComponentData> Components;

        static std::optional<DataAsset> LoadFromFile(const std::string& Path);
    };
}
