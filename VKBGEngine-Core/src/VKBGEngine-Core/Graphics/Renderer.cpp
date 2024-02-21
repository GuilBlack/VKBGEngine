#include "Renderer.h"
#include "Window.h"
#include "RenderContext.h"
#include "SwapChain.h"

namespace vkbg
{
Renderer::Renderer(Window* window, RenderContext* context)
    :m_Window(window), m_Context(context)
{
    RecreateSwapChain();
    CreateCommandBuffers();
}

Renderer::~Renderer()
{
    FreeCommandBuffers();
    delete m_SwapChain;
}

bool Renderer::BeginFrame(VkCommandBuffer& commandBufferToUse)
{
    assert(m_IsFrameStarted == false && "Can't start a frame if a frame is already recording");
    VkResult result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return false;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("Failed to acquire next swap chain image");

    m_IsFrameStarted = true;

    VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("A command buffer has failed to begin recording");

    commandBufferToUse = commandBuffer;
    return true;
}

void Renderer::EndFrame()
{
    assert(m_IsFrameStarted && "Can't end a frame if nothing is recording");
    VkCommandBuffer commandBuffer = GetCurrentCommandBuffer();

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("A command buffer has failed to end recording");

    VkResult result = m_SwapChain->SubmitCommandBuffers(&commandBuffer, &m_CurrentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasWindowResized())
    {
        m_Window->ResetWindowResizeFlag();
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS)
        throw std::runtime_error("Failed to present swap chain image");

    m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    m_IsFrameStarted = false;
}

void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_IsFrameStarted && "Can't call this function if the frame isn't started");
    assert(commandBuffer == GetCurrentCommandBuffer() && "The given command buffer doesn't match the buffer for this frame");

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { .1f, .1f, .1f, 1.f };
    clearValues[1].depthStencil = { 1.f, 0 };

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = m_SwapChain->GetRenderPass(),
        .framebuffer = m_SwapChain->GetFramebuffer(m_CurrentImageIndex),
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
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{
        .x = 0.f,
        .y = 0.f,
        .width = (float)m_SwapChain->GetSwapChainExtent().width,
        .height = (float)m_SwapChain->GetSwapChainExtent().height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };
    VkRect2D scissor{ {0, 0}, m_SwapChain->GetSwapChainExtent() };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(m_IsFrameStarted && "Can't call this function if the frame isn't started");
    assert(commandBuffer == GetCurrentCommandBuffer() && "The given command buffer doesn't match the buffer for this frame");

    vkCmdEndRenderPass(commandBuffer);
}

VkRenderPass Renderer::GetSwapChainRenderPass() const
{
    return m_SwapChain->GetRenderPass();
}

void Renderer::CreateCommandBuffers()
{
    m_CommandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_Context->GetGraphicsCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = (uint32_t)m_CommandBuffers.size(),
    };

    if (vkAllocateCommandBuffers(m_Context->GetLogicalDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("Failed to allocate command buffers");
}

void Renderer::FreeCommandBuffers()
{
    vkFreeCommandBuffers(
        m_Context->GetLogicalDevice(),
        m_Context->GetGraphicsCommandPool(), (uint32_t)m_CommandBuffers.size(),
        m_CommandBuffers.data()
    );
    m_CommandBuffers.clear();
}

void Renderer::RecreateSwapChain()
{
    auto extent = m_Window->GetExtent();
    while (extent.height == 0 || extent.width == 0)
    {
        glfwWaitEvents();
        extent = m_Window->GetExtent();
    }

    // wait for everything on the GPU to finish its executuion 
    // so that we can recreate the swap chain
    vkDeviceWaitIdle(m_Context->GetLogicalDevice());

    if (m_SwapChain == nullptr)
        m_SwapChain = new SwapChain(m_Context, extent);
    else
    {
        SwapChain* oldSwapChain = m_SwapChain;
        m_SwapChain = new SwapChain(m_Context, extent, m_SwapChain);

        if (oldSwapChain->CompareSwapChainFormats(m_SwapChain) == false)
        {
            delete oldSwapChain;
            throw std::runtime_error("Swap chain image or depth format has changed");
        }
        delete oldSwapChain;
    }
}
}
