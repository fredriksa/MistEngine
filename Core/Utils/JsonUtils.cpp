#include "JsonUtils.h"

namespace Core
{
    nlohmann::json Merge(const nlohmann::json& Base, const nlohmann::json& OverrideValues)
    {
        if (!OverrideValues.is_object())
        {
            return Base;
        }

        nlohmann::json Result = Base;
        for (auto& [Key, Value] : OverrideValues.items())
        {
            if (Result.contains(Key) && Result[Key].is_object() && Value.is_object())
            {
                Result[Key] = Merge(Result[Key], Value);
            }
            else
            {
                Result[Key] = Value;
            }
        }
        return Result;
    }
}
