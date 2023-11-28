#include "pch.h"
#pragma once

// indicies of queue families
struct QueueFamilyIndices {
    // rendering
    std::optional<uint32_t> graphicsFamily;
    // presenting rendered images to window surface
    std::optional<uint32_t> presentFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// properties of the swap chain
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanInitializer
{
    VkSurfaceKHR surface;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device; // logical device

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    // enabling or disabling validation layers
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    void initializeVulkan(GLFWwindow* window);

    void createSurface(GLFWwindow* window);

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();

    // checks of the GPUs for availabilty of some features
    bool isDeviceSuitable(VkPhysicalDevice device);

    // querying swap chain details, they specific for each device
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    // check for extensions suitability of the GPUs
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    // checking for requested validation layers to be suported by system
    bool checkValidationLayerSupport();

    // creating info for debugMessenger
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator);

    VkSampleCountFlagBits getMaxUsableSampleCount();

    // getting required extensions for GLFW and their number
    std::vector<const char*> getRequiredExtensions();

    // finding needed queue families
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    // function for debugging callbakcs(checking warnings, errors etc)
    // VKAPI_ATTR and VKAPI_CALL ensures that this function signature suitable for Vulkan
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData);
};
