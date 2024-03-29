#pragma once

namespace vkbg
{
class SwapChain
{
public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
public:
    SwapChain(class RenderContext* context, VkExtent2D windowExtent);
    SwapChain(class RenderContext* context, VkExtent2D windowExtent, SwapChain* oldSwapChain);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

    void Init();

    VkResult AcquireNextImage(uint32_t* imageIndex);
    VkResult SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

    bool CompareSwapChainFormats(SwapChain* other) const
    {
        return m_SwapChainDepthFormat == other->m_SwapChainDepthFormat
            && m_SwapChainImageFormat == other->m_SwapChainImageFormat;
    }

    // Getters
    VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }
    uint32_t GetWidth() const { return m_SwapChainExtent.width; }
    uint32_t GetHeight() const { return m_SwapChainExtent.height; }
    size_t GetImageCount() const { return m_SwapChainImages.size(); }
    VkRenderPass GetRenderPass() const { return m_RenderPass; }
    VkFramebuffer GetFramebuffer(uint32_t index) { return m_SwapChainFramebuffers[index]; }

private:
    class RenderContext* m_Context;
    VkExtent2D m_WindowExtent;

    VkSwapchainKHR m_SwapChain;
    VkFormat m_SwapChainImageFormat;
    VkFormat m_SwapChainDepthFormat;
    VkExtent2D m_SwapChainExtent;

    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;
    std::vector<VkFramebuffer> m_SwapChainFramebuffers;

    std::vector<VkImage> m_DepthImages;
    std::vector<VkDeviceMemory> m_DepthImageMemories;
    std::vector<VkImageView> m_DepthImageViews;

    VkRenderPass m_RenderPass;

    std::vector<VkSemaphore> m_ImageAvailableSemaphores;
    std::vector<VkSemaphore> m_RenderFinishedSemaphores;
    std::vector<VkFence> m_InFlightFences;
    std::vector<VkFence> m_ImagesInFlight;
    size_t m_CurrentFrame = 0;

    SwapChain* m_OldSwapChain;

private:
    void CreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateDepthResources();
    void CreateFrameBuffer();
    void CreateSyncObjects();

    VkSurfaceFormatKHR PickSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR PickSwapChainPresentMode(const std::vector< VkPresentModeKHR>& availablePresentModes);
    VkExtent2D PickSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat FindDepthFormat();
};
}
