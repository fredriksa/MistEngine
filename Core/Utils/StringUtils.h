#pragma once
#include <string>
#include <algorithm>

namespace Core
{
    inline std::string ToLowercase(std::string Str)
    {
        std::transform(Str.begin(), Str.end(), Str.begin(), ::tolower);
        return Str;
    }
}
