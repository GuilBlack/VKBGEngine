#pragma once

namespace vkbg
{
struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;

    bool IsComplete() { return GraphicsFamily.has_value() && PresentFamily.has_value(); }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class RenderContext
{
public:
#ifdef NDEBUG
    constexpr static bool EnableValidationLayers = false;
#else
    constexpr static bool EnableValidationLayers = true;
#endif // NDEBUG
    RenderContext(class Window* window);
    ~RenderContext();

private:
    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void HasGflwRequiredInstanceExtensions();

    void SetupDebugMessage();

    void CreateSurface();

    void PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device, uint32_t& score);
    QueueFamilyIndices FindQueueFamily(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

private:
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    Window* m_pWindow;
    VkSurfaceKHR m_Surface;

    std::vector<const char*> m_ValidationLayers{
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char*> m_DeviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};
}
