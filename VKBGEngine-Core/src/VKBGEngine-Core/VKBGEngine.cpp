#include "VKBGEngine.h"
#include "Window.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "WindowProps.h"

namespace vkbg
{
void Engine::Init(EngineProps properties)
{
    glfwInit();

    m_Window = new Window(properties.WindowProperties);

    m_RenderContext = new RenderContext(m_Window);

    m_SwapChain = new SwapChain(m_RenderContext, {
            properties.WindowProperties.Width,
            properties.WindowProperties.Height
        });

    CreatePipelineLayout();
    CreatePipeline();
    CreateCommandBuffers();
}

void Engine::Run()
{
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!m_Window->ShouldClose())
    {
        glfwPollEvents();
    }
}

void Engine::Shutdown()
{
    vkDestroyPipelineLayout(m_RenderContext->GetLogicalDevice(), m_PipelineLayout, nullptr);
    
    delete m_Pipeline;
    delete m_SwapChain;
    delete m_RenderContext;
    delete m_Window;
}

void Engine::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    if (vkCreatePipelineLayout(m_RenderContext->GetLogicalDevice(), &createInfo, nullptr, &m_PipelineLayout))
        throw std::runtime_error("Failed to create PipelineLayout");
}

void Engine::CreatePipeline()
{
    PipelineProps pipelineProperties = Pipeline::GetDefaultPipelineProps(
        m_SwapChain->GetWidth(),
        m_SwapChain->GetHeight()
    );

    pipelineProperties.RenderPass = m_SwapChain->GetRenderPass();
    pipelineProperties.PipelineLayout = m_PipelineLayout;

    m_Pipeline = new Pipeline(
        m_RenderContext,
        "res/Shaders/Compiled/Simple.vert.spv",
        "res/Shaders/Compiled/Simple.frag.spv",
        pipelineProperties
    );
}

void Engine::CreateCommandBuffers()
{

}

void Engine::DrawFrame()
{

}

}
