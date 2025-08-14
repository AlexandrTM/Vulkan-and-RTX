#pragma once

#include "pch.h"
#include "ImageManager.h"
#include "VulkanInitializer.h"
#include "Model.h"

class AetherSwapchain
{
public:
    AetherSwapchain(
        VulkanInitializer& vkInit, VkRenderPass objectRenderPass,
        VkImageView msaaView, VkImageView depthView
    );

    void create(const QSize windowSize);
    void recreate(const QSize windowSize);
    void cleanup();

    VkFormat getImageFormat() const { return swapchainImageFormat; }
    VkExtent2D getExtent() const { return swapchainExtent; }
    const std::vector<VkImageView>& getImageViews() const { return swapchainImageViews; }
    const std::vector<VkFramebuffer>& getFramebuffers() const { return swapchainFramebuffers; }

private:
    VulkanInitializer& vkInit;
    VkRenderPass objectRenderPass;

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;

    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, const QSize windowSize);

    void createSwapchainInternal(const QSize windowSize);
    void createImageViews();
    void createFramebuffers(VkImageView msaaView, VkImageView depthView, VkRenderPass renderPass);
};
