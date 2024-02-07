#pragma once

namespace vkbg
{
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

private:
    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT m_DebugMessenger;

    std::vector<const char*> m_ValidationLayers{
        "VK_LAYER_KHRONOS_validation"
    };
};
}
