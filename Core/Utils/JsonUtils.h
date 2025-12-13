#pragma once

#include "../ThirdParty/json.hpp"

namespace Core
{
    nlohmann::json Merge(const nlohmann::json& Base, const nlohmann::json& OverrideValues);
}
