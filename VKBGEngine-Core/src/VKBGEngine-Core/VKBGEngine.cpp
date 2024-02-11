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
        DrawFrame();
    }

    vkDeviceWaitIdle(m_RenderContext->GetLogicalDevice());
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
    m_CommandBuffers.resize(m_SwapChain->GetFrameCount());

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_RenderContext->GetGraphicsCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t)m_CommandBuffers.size(),
    };

    if (vkAllocateCommandBuffers(m_RenderContext->GetLogicalDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers");

    for (uint32_t i = 0; i < m_CommandBuffers.size(); ++i)
    {
        VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
        };

        if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("A command buffer has failed to begin recording");

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { .1f, .1f, .1f, 1.f };
        clearValues[1].depthStencil = { 1.f, 0 };

        VkRenderPassBeginInfo renderPassInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = m_SwapChain->GetRenderPass(),
            .framebuffer = m_SwapChain->GetFramebuffer(i),
            .renderArea = {
                .offset = { 0, 0 },
                .extent = m_SwapChain->GetSwapChainExtent()
            },
            .clearValueCount = 2,
            .pClearValues = clearValues.data()
        };

        // inline because it's a primary command buffer and that the commands
        // are in the render pass itself
        // if we used secondary command buffers, we could have used multiple command buffer
        // in one render pass but we can't mix both of these methods
        vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        m_Pipeline->BindToCommandBuffer(m_CommandBuffers[i]);
        vkCmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(m_CommandBuffers[i]);
        if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
            throw std::runtime_error("A command buffer has failed to end recording");
    }
}

void Engine::DrawFrame()
{
    uint32_t imageIndex;
    VkResult result = m_SwapChain->AcquireNextImage(&imageIndex);

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire next swap chain image");

    result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image");
}

}
