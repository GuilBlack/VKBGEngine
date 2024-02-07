#include "RenderContext.h"
#include "Window.h"
#include "VulkanExtensionHelper.h"

namespace vkbg
{
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

RenderContext::RenderContext(Window* window)
{
    CreateInstance();
    SetupDebugMessage();
}

RenderContext::~RenderContext()
{
    if constexpr (EnableValidationLayers)
    {
        DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }

    vkDestroyInstance(m_Instance, nullptr);
}

void RenderContext::CreateInstance()
{
    if constexpr (EnableValidationLayers)
    {
        if (!CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested, but not available!");
        }
    }

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "VKBGEngineApp",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "VKBGEngine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0
    };

    auto extensions = GetRequiredExtensions();

    VkInstanceCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = (uint32_t)extensions.size(),
        .ppEnabledExtensionNames = extensions.data()
    };

    if constexpr (EnableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

    HasGflwRequiredInstanceExtensions();
}

bool RenderContext::CheckValidationLayerSupport()
{
    std::cout << "Checking for validation layers...\n";
    uint32_t layerCount{};
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (auto* layer : m_ValidationLayers)
    {
        bool layerFound = false;
        
        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layer, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> RenderContext::GetRequiredExtensions()
{
    uint32_t glfwExtensionCount{ 0 };
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if constexpr (EnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void RenderContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = DebugCallback,
        .pUserData = nullptr
    };
}

void RenderContext::HasGflwRequiredInstanceExtensions()
{
    uint32_t extensionCount{ 0 };
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "Available extensions:\n";

    std::unordered_set<std::string> availableExtensions;
    for (const auto& extension : extensions)
    {
        std::cout << '\t' << extension.extensionName << '\n';
        availableExtensions.insert(extension.extensionName);
    }
    
    auto glfwRequiredExtensions = GetRequiredExtensions();
    std::cout << "Required extensions:\n";
    for (auto glfwRequiredExtension : glfwRequiredExtensions)
    {
        std::cout << "\t" << glfwRequiredExtension << "\n";
        if (availableExtensions.find(glfwRequiredExtension) == availableExtensions.end())
            throw std::runtime_error("Missing required GLFW extension");
    }
}

void RenderContext::SetupDebugMessage()
{
    if (EnableValidationLayers == false)
        return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger");
}

}
