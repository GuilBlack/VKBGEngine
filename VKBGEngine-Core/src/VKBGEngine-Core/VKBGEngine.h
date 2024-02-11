#pragma once
#include "WindowProps.h"

namespace vkbg
{
struct EngineProps
{
    WindowProps WindowProperties;
};

class Engine
{
public:
    ~Engine() = default;

    static Engine& Instance()
    {
        static Engine instance;
        return instance;
    }

    void Init(EngineProps properties);
    void Run();
    void Shutdown();

private:
    Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    void CreatePipelineLayout();
    void CreatePipeline();
    void CreateCommandBuffers();

    void DrawFrame();

private:
    class Window* m_Window{ nullptr };
    class RenderContext* m_RenderContext{ nullptr };
    class SwapChain* m_SwapChain{ nullptr };
    class Pipeline* m_Pipeline{ nullptr };
    VkPipelineLayout m_PipelineLayout;
    std::vector<VkCommandBuffer> m_CommandBuffers;
};
}

