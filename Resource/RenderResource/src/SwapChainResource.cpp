#include "SwapchainResource.hpp"
#include "Logger.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
void SwapChainResource::InitRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    QuerySurfaceSupport(context);
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{};
    swapChainCreateInfo.setOldSwapchain(nullptr)
        .setSurface(Surface)
        .setMinImageCount(SurfaceInfo.Capabilities.minImageCount)
        .setImageFormat(SurfaceInfo.SurfaceFormat.format)
        .setImageColorSpace(SurfaceInfo.SurfaceFormat.colorSpace)
        .setImageExtent(SurfaceInfo.Capabilities.currentExtent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst |
                       vk::ImageUsageFlagBits::eTransferSrc)
        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(SurfaceInfo.PresentMode)
        .setClipped(vk::True);
    SwapChain = device.createSwapchainKHR(swapChainCreateInfo);
    SwapChainImages = device.getSwapchainImagesKHR(SwapChain);
    SwapChainImageViews.resize(SwapChainImages.size());
    for (size_t i = 0; i < SwapChainImages.size(); ++i)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo{};
        imageViewCreateInfo.setImage(SwapChainImages[i])
            .setViewType(vk::ImageViewType::e2D)
            .setFormat(SurfaceInfo.SurfaceFormat.format)
            .setSubresourceRange(vk::ImageSubresourceRange{}
                                     .setAspectMask(vk::ImageAspectFlagBits::eColor)
                                     .setBaseMipLevel(0)
                                     .setLevelCount(1)
                                     .setBaseArrayLayer(0)
                                     .setLayerCount(1));
        SwapChainImageViews[i] = device.createImageView(imageViewCreateInfo);
    }
}
void SwapChainResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    for (auto &imageView : SwapChainImageViews)
    {
        device.destroyImageView(imageView);
    }
    device.destroySwapchainKHR(SwapChain);
}
void SwapChainResource::CreateSurface(std::shared_ptr<Context> context)
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
    // windows
    vk::Win32SurfaceCreateInfoKHR createInfo{};
    createInfo.setHinstance(HInstance).setHwnd(Hwnd);
    Surface = context->Instance.get().createWin32SurfaceKHR(createInfo);
#elifdef VK_USE_PLATFORM_METAL_EXT
    // macos
    vk::MacOSSurfaceCreateInfoMVK createInfo{};
#elifdef VK_USE_PLATFORM_ANDROID_KHR
    // android
    vk::AndroidSurfaceCreateInfoKHR createInfo{};
#elifdef VK_USE_PLATFORM_XCB_KHR
    // linux xcb
    vk::XcbSurfaceCreateInfoKHR createInfo{};
#elifdef VK_USE_PLATFORM_WAYLAND_KHR
    // linux xlib
    vk::WaylandSurfaceCreateInfoKHR createInfo{};
#elifdef VK_USE_PLATFORM_XLIB_KHR
    // linux xlib
    vk::XlibSurfaceCreateInfoKHR createInfo{};
#endif
}
void SwapChainResource::QuerySurfaceSupport(std::shared_ptr<Context> context)
{
    auto physicalDevice = context->PhysicalDevice;
    auto device = context->Device.get();
    auto formats = physicalDevice.getSurfaceFormatsKHR(context->Surface.get());
    auto presentModes = physicalDevice.getSurfacePresentModesKHR(context->Surface.get());
    SurfaceInfo.Capabilities = physicalDevice.getSurfaceCapabilitiesKHR(context->Surface.get());
    // surface format
    SurfaceInfo.SurfaceFormat = formats[0];
    bool foundFormat = false;
    for (const auto &candidateFormat : SurfaceInfo.candidatesFormats)
    {
        for (const auto &availableFormat : formats)
        {
            if (availableFormat.format == candidateFormat.format &&
                availableFormat.colorSpace == candidateFormat.colorSpace)
            {
                SurfaceInfo.SurfaceFormat = availableFormat;
                foundFormat = true;
                break;
            }
        }
        if (foundFormat)
            break;
    }
    // present mode
    SurfaceInfo.PresentMode = presentModes[0];
    bool foundPresentMode = false;
    for (const auto &candidatePresentMode : SurfaceInfo.candidatePresentModes)
    {
        for (const auto &availablePresentMode : presentModes)
        {
            if (availablePresentMode == candidatePresentMode)
            {
                SurfaceInfo.PresentMode = availablePresentMode;
                foundPresentMode = true;
                break;
            }
        }
        if (foundPresentMode)
            break;
    }
    // Log info
    LogInfo("Selected Surface Format: Format={}, ColorSpace={}", vk::to_string(SurfaceInfo.SurfaceFormat.format),
            vk::to_string(SurfaceInfo.SurfaceFormat.colorSpace));
    LogInfo("Selected Present Mode: {}", vk::to_string(SurfaceInfo.PresentMode));
    LogInfo("Surface Capabilities: minImageCount={}, maxImageCount={}, currentExtent=({}, {}), minImageExtent=({}, "
            "{}), maxImageExtent=({}, {}), maxImageArrayLayers={}",
            SurfaceInfo.Capabilities.minImageCount, SurfaceInfo.Capabilities.maxImageCount,
            SurfaceInfo.Capabilities.currentExtent.width, SurfaceInfo.Capabilities.currentExtent.height,
            SurfaceInfo.Capabilities.minImageExtent.width, SurfaceInfo.Capabilities.minImageExtent.height,
            SurfaceInfo.Capabilities.maxImageExtent.width, SurfaceInfo.Capabilities.maxImageExtent.height,
            SurfaceInfo.Capabilities.maxImageArrayLayers);
}
} // namespace MEngine::Resource