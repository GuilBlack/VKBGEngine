#include "VKBGEngine.h"
#include <GLFW/glfw3.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
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
