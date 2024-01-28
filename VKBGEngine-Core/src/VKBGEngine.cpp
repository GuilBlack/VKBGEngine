#include "VKBGEngine.h"
#include <GLFW/glfw3.h>
#include "Helper.h"

void VKBGEngine::Init()
{
    Helper::Print("Hello World!\n");
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_Window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
}

void VKBGEngine::Run()
{
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
