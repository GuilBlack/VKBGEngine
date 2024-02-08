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

    bool ShouldClose() const;

    void CreateSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
    void Init();
    void Destroy();
    WindowProps m_Properties;
    struct GLFWwindow* m_Window{ nullptr };
};
}

