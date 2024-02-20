#include "VKBGEngine.h"
#include "Window.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderContext.h"
#include "Graphics/SwapChain.h"
#include "Graphics/Model.h"
#include "WindowProps.h"

namespace vkbg
{
void Engine::Init(EngineProps properties)
{
    glfwInit();

    m_Window = new Window(properties.WindowProperties);

    m_RenderContext = new RenderContext(m_Window);

    //m_SwapChain = new SwapChain(m_RenderContext, {
    //        properties.WindowProperties.Width,
    //        properties.WindowProperties.Height
    //    });

    LoadModels();
    CreatePipelineLayout();
    RecreateSwapChain();
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
    delete m_Model;
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
    assert(m_SwapChain != nullptr && "SwapChain wasn't initialized");
    assert(m_PipelineLayout != nullptr && "SwapChain wasn't initialized");

    PipelineProps pipelineProperties{};
    Pipeline::GetDefaultPipelineProps(pipelineProperties);

    pipelineProperties.RenderPass = m_SwapChain->GetRenderPass();
    pipelineProperties.PipelineLayout = m_PipelineLayout;
    
    delete m_Pipeline;
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
}

void Engine::LoadModels()
{
    std::vector<Model::Vertex> vertices{
        {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
    };
    m_Model = new Model(m_RenderContext, vertices);
}

void Engine::DrawFrame()
{
    uint32_t imageIndex;
    VkResult result = m_SwapChain->AcquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire next swap chain image");

    RecordCommandBuffer(imageIndex);
    result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasWindowResized())
    {
        m_Window->ResetWindowResizeFlag();
        RecreateSwapChain();
        return;
    }

    if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image");
}

void Engine::FreeCommandBuffers()
{
    vkFreeCommandBuffers(
        m_RenderContext->GetLogicalDevice(),
        m_RenderContext->GetGraphicsCommandPool(), (uint32_t)m_CommandBuffers.size(),
        m_CommandBuffers.data()
    );
    m_CommandBuffers.clear();
}

void Engine::RecreateSwapChain()
{
    auto extent = m_Window->GetExtent();
    while (extent.height == 0 || extent.width == 0)
    {
        glfwWaitEvents();
        extent = m_Window->GetExtent();
    }

    // wait for everything on the GPU to finish its executuion 
    // so that we can recreate the swap chain
    vkDeviceWaitIdle(m_RenderContext->GetLogicalDevice());

    if (m_SwapChain == nullptr)
        m_SwapChain = new SwapChain(m_RenderContext, extent);
    else
    {
        m_SwapChain = new SwapChain(m_RenderContext, extent, m_SwapChain);
        if (m_SwapChain->GetFrameCount() != m_CommandBuffers.size())
        {
            FreeCommandBuffers();
            CreateCommandBuffers();
        }
    }

    CreatePipeline();
}

void Engine::RecordCommandBuffer(int32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("A command buffer has failed to begin recording");

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { .1f, .1f, .1f, 1.f };
    clearValues[1].depthStencil = { 1.f, 0 };

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_SwapChain->GetRenderPass(),
        .framebuffer = m_SwapChain->GetFramebuffer(imageIndex),
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
    vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{
        .x = 0.f,
        .y = 0.f,
        .width = (float)m_SwapChain->GetSwapChainExtent().width,
        .height = (float)m_SwapChain->GetSwapChainExtent().height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkRect2D scissor{ {0, 0}, m_SwapChain->GetSwapChainExtent() };

    vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

    m_Pipeline->BindToCommandBuffer(m_CommandBuffers[imageIndex]);
    m_Model->Bind(m_CommandBuffers[imageIndex]);
    m_Model->Draw(m_CommandBuffers[imageIndex]);

    vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
    if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
        throw std::runtime_error("A command buffer has failed to end recording");
}


}
