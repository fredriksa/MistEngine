#include "WorldEnvironment.h"

namespace Core
{
    nlohmann::json DayNightColors::ToJson() const
    {
        nlohmann::json Json;
        Json["night"] = {Night.x, Night.y, Night.z};
        Json["sunrise"] = {Sunrise.x, Sunrise.y, Sunrise.z};
        Json["day"] = {Day.x, Day.y, Day.z};
        Json["sunset"] = {Sunset.x, Sunset.y, Sunset.z};
        return Json;
    }

    DayNightColors DayNightColors::FromJson(const nlohmann::json& Json)
    {
        DayNightColors Colors;

        if (Json.contains("night") && Json["night"].is_array() && Json["night"].size() == 3)
        {
            Colors.Night = {Json["night"][0], Json["night"][1], Json["night"][2]};
        }
        if (Json.contains("sunrise") && Json["sunrise"].is_array() && Json["sunrise"].size() == 3)
        {
            Colors.Sunrise = {Json["sunrise"][0], Json["sunrise"][1], Json["sunrise"][2]};
        }
        if (Json.contains("day") && Json["day"].is_array() && Json["day"].size() == 3)
        {
            Colors.Day = {Json["day"][0], Json["day"][1], Json["day"][2]};
        }
        if (Json.contains("sunset") && Json["sunset"].is_array() && Json["sunset"].size() == 3)
        {
            Colors.Sunset = {Json["sunset"][0], Json["sunset"][1], Json["sunset"][2]};
        }

        return Colors;
    }

    WorldEnvironment::WorldEnvironment()
    {
    }

    nlohmann::json WorldEnvironment::ToJson() const
    {
        nlohmann::json Json;

        nlohmann::json TimeJson;
        TimeJson["hours"] = Time.GetHourOfDay();
        TimeJson["minutes"] = Time.GetMinutes();

        Json["time"] = TimeJson;
        Json["staticTime"] = Time.IsStaticTime();
        Json["timeScale"] = Time.GetTimeScale();
        Json["colors"] = Colors.ToJson();

        return Json;
    }

    WorldEnvironment WorldEnvironment::FromJson(const nlohmann::json& Json)
    {
        WorldEnvironment Env;

        if (Json.contains("time") && Json["time"].is_object())
        {
            const nlohmann::json& TimeJson = Json["time"];
            const int Hours = TimeJson.value("hours", 12);
            const int Minutes = TimeJson.value("minutes", 0);
            Env.Time.SetTime(Hours, Minutes);
        }

        Env.Time.SetStaticTime(Json.value("staticTime", false));
        Env.Time.SetTimeScale(Json.value("timeScale", 1.0f));

        if (Json.contains("colors"))
        {
            Env.Colors = DayNightColors::FromJson(Json["colors"]);
        }

        return Env;
    }

    WorldEnvironment WorldEnvironment::Default()
    {
        return WorldEnvironment();
    }
}
