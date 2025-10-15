#pragma once

#include "pch.h"

// indices of queue families
struct QueueFamilyIndices {
    // rendering
    std::optional<uint32_t> graphicsFamily;
    // presenting rendered images to window surface
    std::optional<uint32_t> presentFamily;

    bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

// properties of the swap chain
struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VulkanInitializer
{
    VkInstance instance;
    VkDevice device; // logical device
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VmaAllocator vmaAllocator;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    QueueFamilyIndices queueFamilyIndices;
    VkCommandPool commandPool;
    VkSurfaceKHR surface;

    VkDescriptorPool          descriptorPool;
    VkDescriptorSetLayout     descriptorSetLayout;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkSampleCountFlagBits colorSamples;
    VkSampleCountFlagBits depthSamples;

    // enabling or disabling validation layers
    #ifdef NDEBUG
        const bool enableValidationLayers = false;
    #else
        const bool enableValidationLayers = true;
    #endif

    void initializeVulkan(QVulkanInstance* qInstance);

    void initVMA();

    void createInstance();
    void setupDebugMessenger();
    void pickPhysicalDevice();
    void createLogicalDevice();
    // for specific queue family
    void createCommandPool();

    // checks of the GPUs for availability of some features
    bool isDeviceSuitable(VkPhysicalDevice device);

    // querying swap chain details, they specific for each device
    SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device) const;

    // check for extensions suitability of the GPUs
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    // checking for requested validation layers to be supported by system
    bool checkValidationLayerSupport();

    // creating info for debugMessenger
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDebugUtilsMessengerEXT* pDebugMessenger
    );
    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    );

    void findMaxUsableSampleCount(VkPhysicalDevice physicalDevice);

    std::vector<const char*> getRequiredExtensions() const;

    // finding needed queue families
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) const;

    // function for debugging callbacks(checking warnings, errors etc)
    // VKAPI_ATTR and VKAPI_CALL ensures that this function signature suitable for Vulkan
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType, 
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
    );
};
