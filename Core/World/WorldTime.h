#pragma once

#include <chrono>
#include <SFML/Graphics/Color.hpp>

namespace Core
{
    class WorldTime
    {
    public:
        using GameHours = std::chrono::duration<float, std::ratio<3600>>;
        using GameSeconds = std::chrono::duration<float>;

        WorldTime();

        void Tick(float DeltaTimeS);

        float GetHours() const;
        int GetHourOfDay() const;
        int GetMinutes() const;

        void SetTime(GameHours Time);
        void SetTime(int Hours, int Minutes = 0);

        float GetTimeScale() const { return TimeScale; }
        void SetTimeScale(float Scale) { TimeScale = Scale; }

        bool IsStaticTime() const { return bStaticTime; }
        void SetStaticTime(bool bStatic) { bStaticTime = bStatic; }

        sf::Color GetAmbientColor() const;
        sf::Color GetSkyColor() const;

    private:
        GameHours CurrentTime{12.0f};
        float TimeScale = 1.0f;
        bool bStaticTime = false;

        sf::Color InterpolateColor(float Hour, const sf::Color& Color1, const sf::Color& Color2, float Hour1, float Hour2) const;
    };
}
