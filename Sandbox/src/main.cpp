
#include "Engine.h"

int main()
{
    Forge::Engine engine;

    if (!engine.Initialize())
        return 1;

    engine.Run();
    engine.Shutdown();

    return 0;
}
