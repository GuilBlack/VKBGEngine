#include "VKBGEInclude.h"

int main()
{
    static constexpr uint32_t WIDTH{ 800 };
    static constexpr uint32_t HEIGHT{ 600 };
    static constexpr char TITLE[11]{ "VKBGEngine" };
    vkbg::Engine& engine = vkbg::Engine::Instance();
    engine.Init({WIDTH, HEIGHT, TITLE});

    try
    {
        engine.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        engine.Shutdown();
        return EXIT_FAILURE;
    }
    engine.Shutdown();
    return EXIT_SUCCESS;
}
