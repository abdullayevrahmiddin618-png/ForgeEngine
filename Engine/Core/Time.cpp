
#include "Time.h"

#include <algorithm>

namespace Forge
{
    Time::Clock::time_point Time::s_StartTime{};
    Time::Clock::time_point Time::s_LastFrameTime{};

    double Time::s_DeltaTime = 0.0;
    double Time::s_ElapsedTime = 0.0;

    float Time::s_FPS = 0.0f;
    std::uint64_t Time::s_FrameCount = 0;
    std::uint32_t Time::s_TargetFPS = 0;

    void Time::Initialize()
    {
        const auto now = Clock::now();

        s_StartTime = now;
        s_LastFrameTime = now;

        s_DeltaTime = 0.0;
        s_ElapsedTime = 0.0;
        s_FPS = 0.0f;
        s_FrameCount = 0;
    }

    void Time::Update()
    {
        const auto now = Clock::now();

        s_DeltaTime = std::chrono::duration<double>(
            now - s_LastFrameTime
        ).count();

        // Prevent unusually large frame times after pauses.
        s_DeltaTime = std::clamp(s_DeltaTime, 0.0, 0.25);

        s_ElapsedTime = std::chrono::duration<double>(
            now - s_StartTime
        ).count();

        s_LastFrameTime = now;
        ++s_FrameCount;

        if (s_DeltaTime > 0.0)
            s_FPS = static_cast<float>(1.0 / s_DeltaTime);
    }

    float Time::GetDeltaTime()
    {
        return static_cast<float>(s_DeltaTime);
    }

    float Time::GetElapsedTime()
    {
        return static_cast<float>(s_ElapsedTime);
    }

    float Time::GetFPS()
    {
        return s_FPS;
    }

    double Time::GetDeltaTimePrecise()
    {
        return s_DeltaTime;
    }

    double Time::GetElapsedTimePrecise()
    {
        return s_ElapsedTime;
    }

    std::uint64_t Time::GetFrameCount()
    {
        return s_FrameCount;
    }

    void Time::SetTargetFPS(std::uint32_t fps)
    {
        s_TargetFPS = fps;
    }

    std::uint32_t Time::GetTargetFPS()
    {
        return s_TargetFPS;
    }
}
