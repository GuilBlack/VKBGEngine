#pragma once

namespace vkbg
{
class SwapChain
{
public:
    static constexpr uint32_t MAX_CONCURRENT_FRAMES = 2;
public:
    SwapChain(class RenderContext* context, VkExtent2D windowExtent);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    void operator=(const SwapChain&) = delete;

private:
    class RenderContext* m_Context;
    VkExtent2D m_WindowExtent;

    VkSwapchainKHR m_SwapChain;
    VkFormat m_SwapChainImageFormat;
    VkExtent2D m_SwapChainExtent;
    std::vector<VkImage> m_SwapChainImages;
    std::vector<VkImageView> m_SwapChainImageViews;

private:
    void CreateSwapChain();
    VkSurfaceFormatKHR PickSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR PickSwapChainPresentMode(const std::vector< VkPresentModeKHR>& availablePresentModes);
    VkExtent2D PickSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void CreateImageViews();
};
}
