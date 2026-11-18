
#pragma once

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

    private:
        static float s_DeltaTime;
        static float s_ElapsedTime;
        static float s_FPS;
    };
}
