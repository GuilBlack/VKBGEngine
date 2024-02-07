#pragma once

namespace vkbg
{

class Engine
{
public:
    ~Engine() = default;

    static Engine& Instance()
    {
        static Engine instance;
        return instance;
    }

    void Init(struct WindowProps properties);
    void Run();
    void Shutdown();

private:
    Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

private:
    class Window* m_Window{ nullptr };
    class Pipeline* m_Pipeline{ nullptr };
    class RenderContext* m_RenderContext{ nullptr };
};

}

