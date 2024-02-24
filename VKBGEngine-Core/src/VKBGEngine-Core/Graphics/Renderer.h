#pragma once

namespace vkbg
{
class Renderer
{
public:
    Renderer(class Window* window, class RenderContext* context);
    ~Renderer();

    bool BeginFrame(VkCommandBuffer& commandBufferToUse);
    void EndFrame();
    void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

    // Getters
    bool IsFrameStarted() { return m_IsFrameStarted; }
    VkCommandBuffer GetCurrentCommandBuffer() const 
    {
        assert(m_IsFrameStarted && "Cannot get command buffer if frame isn't in progress");
        return m_CommandBuffers[m_CurrentFrameIndex];
    }
    uint32_t GetCurrentFrameIndex()
    {
        assert(m_IsFrameStarted && "Cannot get frame index if frame isn't in progress");
        return m_CurrentFrameIndex;
    }
    VkRenderPass GetSwapChainRenderPass() const;
    VkExtent2D GetSwapChainExtent() const;
    float GetAspectRatio() const;

private:
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

    void CreateCommandBuffers();
    void FreeCommandBuffers();

    void RecreateSwapChain();

private:
    // references
    class Window* m_Window{ nullptr };
    class RenderContext* m_Context{ nullptr };

private:
    class SwapChain* m_SwapChain{ nullptr };
    std::vector<VkCommandBuffer> m_CommandBuffers;

    uint32_t m_CurrentImageIndex{ 0 }; // use swap chain next image
    uint32_t m_CurrentFrameIndex{ 0 }; // use frame in flight
    bool m_IsFrameStarted{ false };
};
}
