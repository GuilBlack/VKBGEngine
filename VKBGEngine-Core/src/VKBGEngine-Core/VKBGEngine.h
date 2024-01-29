#pragma once
class VKBGEngine
{
public:
    VKBGEngine()
        : m_Window(nullptr)
    {}

    static VKBGEngine& Instance()
    {
        static VKBGEngine instance;
        return instance;
    }

    void Init();
    void Run();
    void Shutdown();

private:
    struct GLFWwindow* m_Window;
};

