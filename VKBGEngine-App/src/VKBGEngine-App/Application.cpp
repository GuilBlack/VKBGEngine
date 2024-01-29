#include "VKBGEInclude.h"

int main()
{
    VKBGEngine& engine = VKBGEngine::Instance();
    engine.Init();
    engine.Run();
    engine.Shutdown();
    return 0;
}
