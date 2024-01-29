#include "VKBGEngine.h"
#include "Helper.h"

void VKBGEngine::Init()
{
    Helper::Print("Hello World!\n");
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";
}

void VKBGEngine::Run()
{
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();
    }
}

void VKBGEngine::Shutdown()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}
