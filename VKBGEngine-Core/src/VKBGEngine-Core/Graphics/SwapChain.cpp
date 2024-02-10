#include "SwapChain.h"
#include "RenderContext.h"

namespace vkbg
{
SwapChain::SwapChain(RenderContext* context, VkExtent2D windowExtent)
    : m_Context{context}, m_WindowExtent{windowExtent}
{
    CreateSwapChain();
    CreateImageViews();
}

SwapChain::~SwapChain()
{
    for (auto imageView : m_SwapChainImageViews)
        vkDestroyImageView(m_Context->GetLogicalDevice(), imageView, nullptr);

    if (m_SwapChain != nullptr)
    {
        vkDestroySwapchainKHR(m_Context->GetLogicalDevice(), m_SwapChain, nullptr);
        m_SwapChain = nullptr;
    }
}

void SwapChain::CreateSwapChain()
{
    SwapChainSupportDetails swapChainSupport{ m_Context->GetSwapChainSupport() };

    VkSurfaceFormatKHR surfaceFormat = PickSwapChainSurfaceFormat(swapChainSupport.Formats);
    VkPresentModeKHR presentMode = PickSwapChainPresentMode(swapChainSupport.PresentModes);
    VkExtent2D extent = PickSwapChainExtent(swapChainSupport.Capabilities);

    uint32_t imageCount{ swapChainSupport.Capabilities.minImageCount + 1 };
    if (swapChainSupport.Capabilities.maxImageCount > 0
        && imageCount > swapChainSupport.Capabilities.maxImageCount)
        imageCount = swapChainSupport.Capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = m_Context->GetSurface(),
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1, // useful if we do things in VR. else, 1 is good enough
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = swapChainSupport.Capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    QueueFamilyIndices queueFamilies = m_Context->GetQueueFamilies();
    uint32_t queueFamilyIndices[]{
        queueFamilies.GraphicsFamily.value(),
        queueFamilies.PresentFamily.value()
    };

    if (queueFamilies.GraphicsFamily.value() != queueFamilies.PresentFamily.value())
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    VkDevice device = m_Context->GetLogicalDevice();

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
        throw std::runtime_error("Failed to create swapchain!");

    vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
    m_SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_SwapChainImages.data());

    m_SwapChainImageFormat = surfaceFormat.format;
    m_SwapChainExtent = extent;
}

VkSurfaceFormatKHR SwapChain::PickSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR SwapChain::PickSwapChainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    // VK_PRESENT_MODE_MAILBOX_KHR = Never lets the GPU idle
    // Triple buffering or more, it will override the oldest back buffer
    // so that we can always have the latest frame on our screen.
    // Not good for mobile since it consumes a lot of power.
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    // VK_PRESENT_MODE_IMMEDIATE_KHR = No sync mode and can cause screen tearing
    // for (const auto &availablePresentMode : availablePresentModes) {
    //   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //     std::cout << "Present mode: Immediate" << std::endl;
    //     return availablePresentMode;
    //   }
    // }

    std::cout << "Present mode: V-Sync" << std::endl;
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::PickSwapChainExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return capabilities.currentExtent;

    VkExtent2D actualExtent = m_WindowExtent;
    actualExtent.width = std::clamp(
        actualExtent.width, 
        capabilities.minImageExtent.width, 
        capabilities.maxImageExtent.width
    );

    actualExtent.height = std::clamp(
        actualExtent.height,
        capabilities.minImageExtent.height,
        capabilities.maxImageExtent.height
    );

    return actualExtent;
}

void SwapChain::CreateImageViews()
{
    m_SwapChainImageViews.resize(m_SwapChainImages.size());

    for (size_t i = 0; i < m_SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_SwapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = m_SwapChainImageFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1 // Great for VR else should be 1
            }
        };

        if (vkCreateImageView(m_Context->GetLogicalDevice(), &viewInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create texture image view!");
    }
}
}
