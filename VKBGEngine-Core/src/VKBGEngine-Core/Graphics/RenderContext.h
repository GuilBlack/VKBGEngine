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

    VkDevice GetLogicalDevice() const { return m_Device; }
    SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
    VkSurfaceKHR GetSurface() { return m_Surface; }

    QueueFamilyIndices GetQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }

private:

#pragma region Initialization Code
    void CreateInstance();
    bool CheckValidationLayerSupport();
    std::vector<const char*> GetRequiredExtensions();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void HasGflwRequiredInstanceExtensions();

    void SetupDebugMessage();

    void CreateSurface();

    void PickPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device, uint32_t& score);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

    void CreateLogicalDevice();

    void CreateCommandPool();
#pragma endregion


private:
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDebugUtilsMessengerEXT m_DebugMessenger;
    class Window* m_pWindow;
    
    VkSurfaceKHR m_Surface;
    VkDevice m_Device;
    VkQueue m_GraphicsQueue;
    VkCommandPool m_GraphicsCommandPool;
    VkQueue m_PresentQueue;

    std::vector<const char*> m_ValidationLayers{
        "VK_LAYER_KHRONOS_validation"
    };
    std::vector<const char*> m_DeviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};
}
