#pragma once

#include "WorldTime.h"
#include "../ThirdParty/json.hpp"
#include <SFML/System/Vector3.hpp>

namespace Core
{
    struct DayNightColors
    {
        sf::Vector3f Night = {0.255f, 0.255f, 0.425f};
        sf::Vector3f Sunrise = {1.0f, 0.6f, 0.4f};
        sf::Vector3f Day = {1.0f, 1.0f, 1.0f};
        sf::Vector3f Sunset = {1.0f, 0.6f, 0.4f};

        nlohmann::json ToJson() const;
        static DayNightColors FromJson(const nlohmann::json& Json);
    };

    class WorldEnvironment
    {
    public:
        WorldEnvironment();

        WorldTime& GetTime() { return Time; }
        const WorldTime& GetTime() const { return Time; }

        DayNightColors& GetDayNightColors() { return Colors; }
        const DayNightColors& GetDayNightColors() const { return Colors; }

        nlohmann::json ToJson() const;
        static WorldEnvironment FromJson(const nlohmann::json& Json);
        static WorldEnvironment Default();

    private:
        WorldTime Time;
        DayNightColors Colors;
    };
}
