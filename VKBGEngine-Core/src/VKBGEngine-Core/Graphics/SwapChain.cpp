#include "SwapChain.h"
#include "RenderContext.h"

namespace vkbg
{
SwapChain::SwapChain(RenderContext* context, VkExtent2D windowExtent)
    : m_Context{context}, m_WindowExtent{windowExtent}
{
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateDepthResources();
    CreateFrameBuffer();
    CreateSyncObjects();
}

SwapChain::~SwapChain()
{
    VkDevice device = m_Context->GetLogicalDevice();

    for (auto imageView : m_SwapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);
    m_SwapChainImageViews.clear();

    if (m_SwapChain != nullptr)
    {
        vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
        m_SwapChain = nullptr;
    }

    for (size_t i = 0; i < m_DepthImages.size(); ++i)
    {
        vkDestroyImageView(device, m_DepthImageViews[i], nullptr);
        vkDestroyImage(device, m_DepthImages[i], nullptr);
        vkFreeMemory(device, m_DepthImageMemories[i], nullptr);
    }

    for (auto framebuffer : m_SwapChainFramebuffers)
    {
        vkDestroyFramebuffer(device, framebuffer, nullptr);
    }

    vkDestroyRenderPass(device, m_RenderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
        vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
        vkDestroyFence(device, m_InFlightFences[i], nullptr);
    }
}

VkResult SwapChain::AcquireNextImage(uint32_t* imageIndex)
{
    vkWaitForFences(
        m_Context->GetLogicalDevice(), 
        1, &m_InFlightFences[m_CurrentFrame], 
        VK_TRUE, std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        m_Context->GetLogicalDevice(), m_SwapChain,
        std::numeric_limits<uint64_t>::max(), m_ImageAvailableSemaphores[m_CurrentFrame],
        VK_NULL_HANDLE, imageIndex);

    return result;
}

VkResult SwapChain::SubmitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
    if (m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(
            m_Context->GetLogicalDevice(), 
            1, &m_ImagesInFlight[*imageIndex], 
            VK_TRUE, UINT64_MAX);
    }
    m_ImagesInFlight[*imageIndex] = m_InFlightFences[m_CurrentFrame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = buffers;

    VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(m_Context->GetLogicalDevice(), 1, &m_ImagesInFlight[m_CurrentFrame]);
    if (vkQueueSubmit(m_Context->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) !=
        VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { m_SwapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = imageIndex;

    auto result = vkQueuePresentKHR(m_Context->GetPresentQueue(), &presentInfo);

    m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
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

void SwapChain::CreateRenderPass()
{
    VkAttachmentDescription depthAttachment{
        .format = FindDepthFormat(),
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depthAttachementRef{
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription colorAttachment{
        .format = m_SwapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };
    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpass{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthAttachementRef
    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    };
    
    std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = (uint32_t)attachments.size(),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency,
    };

    if (vkCreateRenderPass(m_Context->GetLogicalDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create render pass!");
    }
}

void SwapChain::CreateDepthResources()
{
    VkFormat depthFormat = FindDepthFormat();
    VkExtent2D swapChainExtent = GetSwapChainExtent();

    m_DepthImages.resize(GetFrameCount());
    m_DepthImageMemories.resize(GetFrameCount());
    m_DepthImageViews.resize(GetFrameCount());

    for (int i = 0; i < m_DepthImages.size(); i++) 
    {
        VkImageCreateInfo imageInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depthFormat,
            .extent = {
                .width = swapChainExtent.width,
                .height = swapChainExtent.height,
                .depth = 1,
            },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        m_Context->CreateImageWithInfo(
            imageInfo,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_DepthImages[i],
            m_DepthImageMemories[i]);

        VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = m_DepthImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = depthFormat,
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        if (vkCreateImageView(m_Context->GetLogicalDevice(), &viewInfo, nullptr, &m_DepthImageViews[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create texture image view!");
    }
}

void SwapChain::CreateFrameBuffer()
{
    m_SwapChainFramebuffers.resize(GetFrameCount());
    for (size_t i = 0; i < GetFrameCount(); i++)
    {
        std::array<VkImageView, 2> attachments{ 
            m_SwapChainImageViews[i], 
            m_DepthImageViews[i] 
        };

        VkExtent2D swapChainExtent = GetSwapChainExtent();
        VkFramebufferCreateInfo framebufferInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = m_RenderPass,
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .width = swapChainExtent.width,
            .height = swapChainExtent.height,
            .layers = 1,
        };

        if (vkCreateFramebuffer(
            m_Context->GetLogicalDevice(),
            &framebufferInfo,
            nullptr,
            &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void SwapChain::CreateSyncObjects()
{
    m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    m_ImagesInFlight.resize(GetFrameCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(
                m_Context->GetLogicalDevice(), 
                &semaphoreInfo, nullptr, 
                &m_ImageAvailableSemaphores[i]) != VK_SUCCESS 
            || vkCreateSemaphore(
                m_Context->GetLogicalDevice(), 
                &semaphoreInfo, nullptr, 
                &m_RenderFinishedSemaphores[i]) != VK_SUCCESS
            || vkCreateFence(
                m_Context->GetLogicalDevice(), 
                &fenceInfo, 
                nullptr, 
                &m_InFlightFences[i]) != VK_SUCCESS
            )
            throw std::runtime_error("failed to create synchronization objects for a frame!");
    }
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

VkFormat SwapChain::FindDepthFormat()
{
    return m_Context->FindSupportedFormat(
        {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT
        },
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
}
