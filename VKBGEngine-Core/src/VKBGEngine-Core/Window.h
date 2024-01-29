#pragma once
namespace vkbg
{
struct WindowProps
{
    uint32_t Width;
    uint32_t Height;
    std::string Title;
};

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

private:
    void Init();
    void Destroy();
    WindowProps m_Properties;
    struct GLFWwindow* m_Window{ nullptr };
};
}

