#include "Window.h"

namespace vkbg
{
bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title.c_str(), nullptr, nullptr);
}

void Window::Destroy()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

}
