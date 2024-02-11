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

    VkExtent2D GetExtent() { return { m_Properties.Width, m_Properties.Height }; }

    bool ShouldClose() const;

    void CreateSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    void Init();
    void Destroy();
    WindowProps m_Properties;
    struct GLFWwindow* m_Window{ nullptr };
};
}

