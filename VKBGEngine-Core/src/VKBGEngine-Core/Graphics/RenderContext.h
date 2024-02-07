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

private:
    VkInstance m_Instance;

    std::vector<const char*> m_ValidationLayers{
        "VK_LAYER_KHRONOS_validation"
    };
};
}
