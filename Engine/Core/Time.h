
#pragma once

#include <chrono>
#include <cstdint>

namespace Forge
{
    class Time
    {
    public:
        static void Initialize();
        static void Update();

        static float GetDeltaTime();
        static float GetElapsedTime();
        static float GetFPS();

        static double GetDeltaTimePrecise();
        static double GetElapsedTimePrecise();

        static std::uint64_t GetFrameCount();

        static void SetTargetFPS(std::uint32_t fps);
        static std::uint32_t GetTargetFPS();

    private:
        using Clock = std::chrono::steady_clock;

        static Clock::time_point s_StartTime;
        static Clock::time_point s_LastFrameTime;

        static double s_DeltaTime;
        static double s_ElapsedTime;

        static float s_FPS;
        static std::uint64_t s_FrameCount;
        static std::uint32_t s_TargetFPS;
    };
}
