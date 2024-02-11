#pragma once

namespace vkbg
{
class SwapChain
{
public:
    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
public:
    SwapChain(class RenderContext* context, VkExtent2D windowExtent);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    void operator=(const SwapChain&) = delete;

    // Getters
    VkExtent2D GetSwapChainExtent() { return m_SwapChainExtent; }
    size_t GetFrameCount() { return m_SwapChainImages.size(); }

private:
    class RenderContext* m_Context;
    VkExtent2D m_WindowExtent;

    VkSwapchainKHR m_SwapChain;
    VkFormat m_SwapChainImageFormat;
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
