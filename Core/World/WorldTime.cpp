#include "WorldTime.h"
#include <cmath>
#include <algorithm>

namespace Core
{
    WorldTime::WorldTime()
        : CurrentTime(12.0f)
        , TimeScale(1.0f)
    {
    }

    void WorldTime::Tick(float DeltaTimeS)
    {
        GameSeconds DeltaSeconds(DeltaTimeS * TimeScale);
        CurrentTime += std::chrono::duration_cast<GameHours>(DeltaSeconds);

        const float TotalHours = CurrentTime.count();
        if (TotalHours >= 24.0f)
        {
            CurrentTime = GameHours(std::fmod(TotalHours, 24.0f));
        }
    }

    float WorldTime::GetHours() const
    {
        return CurrentTime.count();
    }

    int WorldTime::GetHourOfDay() const
    {
        return static_cast<int>(std::floor(CurrentTime.count())) % 24;
    }

    int WorldTime::GetMinutes() const
    {
        const float Hours = CurrentTime.count();
        const float FractionalHour = Hours - std::floor(Hours);
        return static_cast<int>(FractionalHour * 60.0f);
    }

    void WorldTime::SetTime(GameHours Time)
    {
        CurrentTime = Time;
        const float TotalHours = CurrentTime.count();
        if (TotalHours >= 24.0f)
        {
            CurrentTime = GameHours(std::fmod(TotalHours, 24.0f));
        }
    }

    void WorldTime::SetTime(int Hours, int Minutes)
    {
        const float TotalHours = static_cast<float>(Hours) + (static_cast<float>(Minutes) / 60.0f);
        SetTime(GameHours(TotalHours));
    }

    sf::Color WorldTime::GetAmbientColor() const
    {
        const float Hour = GetHours();

        if (Hour >= 6.0f && Hour < 12.0f)
        {
            return InterpolateColor(Hour, sf::Color(100, 100, 150), sf::Color(255, 255, 255), 6.0f, 12.0f);
        }
        else if (Hour >= 12.0f && Hour < 18.0f)
        {
            return InterpolateColor(Hour, sf::Color(255, 255, 255), sf::Color(255, 150, 100), 12.0f, 18.0f);
        }
        else if (Hour >= 18.0f && Hour < 22.0f)
        {
            return InterpolateColor(Hour, sf::Color(255, 150, 100), sf::Color(50, 50, 100), 18.0f, 22.0f);
        }
        else
        {
            return sf::Color(50, 50, 100);
        }
    }

    sf::Color WorldTime::GetSkyColor() const
    {
        const float Hour = GetHours();

        if (Hour >= 5.0f && Hour < 7.0f)
        {
            return InterpolateColor(Hour, sf::Color(25, 25, 50), sf::Color(255, 150, 100), 5.0f, 7.0f);
        }
        else if (Hour >= 7.0f && Hour < 12.0f)
        {
            return InterpolateColor(Hour, sf::Color(255, 150, 100), sf::Color(135, 206, 235), 7.0f, 12.0f);
        }
        else if (Hour >= 12.0f && Hour < 17.0f)
        {
            return sf::Color(135, 206, 235);
        }
        else if (Hour >= 17.0f && Hour < 19.0f)
        {
            return InterpolateColor(Hour, sf::Color(135, 206, 235), sf::Color(255, 100, 50), 17.0f, 19.0f);
        }
        else if (Hour >= 19.0f && Hour < 21.0f)
        {
            return InterpolateColor(Hour, sf::Color(255, 100, 50), sf::Color(25, 25, 50), 19.0f, 21.0f);
        }
        else
        {
            return sf::Color(25, 25, 50);
        }
    }

    sf::Color WorldTime::InterpolateColor(float Hour, const sf::Color& Color1, const sf::Color& Color2, float Hour1, float Hour2) const
    {
        const float T = (Hour - Hour1) / (Hour2 - Hour1);
        const float ClampedT = std::clamp(T, 0.0f, 1.0f);

        const std::uint8_t R = static_cast<std::uint8_t>(Color1.r + (Color2.r - Color1.r) * ClampedT);
        const std::uint8_t G = static_cast<std::uint8_t>(Color1.g + (Color2.g - Color1.g) * ClampedT);
        const std::uint8_t B = static_cast<std::uint8_t>(Color1.b + (Color2.b - Color1.b) * ClampedT);
        const std::uint8_t A = static_cast<std::uint8_t>(Color1.a + (Color2.a - Color1.a) * ClampedT);

        return sf::Color(R, G, B, A);
    }
}
