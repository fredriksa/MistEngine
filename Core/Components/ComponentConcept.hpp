#pragma once

#include <type_traits>

namespace Core
{
    class Component;

    template <typename T>
    concept IsComponent = std::is_base_of_v<Component, T>;
}
