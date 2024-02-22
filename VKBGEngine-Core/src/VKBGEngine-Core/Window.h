#pragma once
#include "WindowProps.h"

namespace vkbg
{

class Window
{
public:
    Window(WindowProps properties) : m_Properties(properties) 
    { 
        Init();
    }
    ~Window()
    {
        Destroy();
    }

    GLFWwindow* GetWindowHandle() const { return m_Window; }
    VkExtent2D GetExtent() { return { m_Properties.Width, m_Properties.Height }; }
    bool WasWindowResized() { return m_WindowResized; }
    void ResetWindowResizeFlag() { m_WindowResized = false; }

    bool ShouldClose() const;

    void CreateSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int32_t width, int32_t height);
    void Init();
    void Destroy();

private:
    WindowProps m_Properties;
    bool m_WindowResized{ false };
    GLFWwindow* m_Window{ nullptr };
};
}

