#include "Window.h"

namespace vkbg
{
void Window::FramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height)
{
    auto vkbgWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    vkbgWindow->m_WindowResized = true;
    vkbgWindow->m_Properties.Width = width;
    vkbgWindow->m_Properties.Height = height;
}

void Window::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_Window = glfwCreateWindow(m_Properties.Width, m_Properties.Height, m_Properties.Title.c_str(), nullptr, nullptr);
    glfwSetWindowSizeLimits(m_Window, 400, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, Window::FramebufferSizeCallback);
}

void Window::Destroy()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::CreateSurface(VkInstance instance, VkSurfaceKHR* surface)
{
    if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
        throw std::runtime_error("Failed to create VkSurfaceKHR");
}
}
