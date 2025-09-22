
#include "RHISurface.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHISurface::RHISurface(VkSurfaceKHR &&surface)
{
    RHISurface(vk::SurfaceKHR(std::move(surface)));
}
RHISurface::RHISurface(vk::SurfaceKHR &&surface)
{
    mSurface = vk::UniqueSurfaceKHR(surface);
    auto PhysicalDevice = RHIContext::Instance().GetPhysicalDevice();

    auto formats = PhysicalDevice.getSurfaceFormatsKHR(mSurface.get());
    auto presentModes = PhysicalDevice.getSurfacePresentModesKHR(mSurface.get());
    auto capabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(mSurface.get());
    std::vector<vk::SurfaceFormatKHR> candidatesFormats = {
        {vk::Format::eR32G32B32A32Sfloat, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
    };
    std::vector<vk::PresentModeKHR> candidatesPresentModes = {
        vk::PresentModeKHR::eMailbox,
        vk::PresentModeKHR::eFifo,
    };
    bool formatFound = false;
    bool presentModeFound = false;
    SurfaceInfo.format = formats[0];
    for (auto &format : candidatesFormats)
    {
        for (auto &supportFormat : formats)
        {
            if (format.format == supportFormat.format && format.colorSpace == supportFormat.colorSpace)
            {
                SurfaceInfo.format = format;
                formatFound = true;
                break;
            }
        }
        if (formatFound)
            break;
    }
    SurfaceInfo.presentMode = presentModes[0];
    for (auto &presentMode : candidatesPresentModes)
    {
        for (auto &supportPresentMode : presentModes)
        {
            if (presentMode == supportPresentMode)
            {
                SurfaceInfo.presentMode = presentMode;
                presentModeFound = true;
                break;
            }
        }
        if (presentModeFound)
            break;
    }
    SurfaceInfo.extent = capabilities.currentExtent;
    SurfaceInfo.imageCount = std::clamp(2u, capabilities.minImageCount, capabilities.maxImageCount);
    SurfaceInfo.imageArrayLayer = std::clamp(1u, 1u, capabilities.maxImageArrayLayers);
    // log
    // std::println("Current Surface Info:");
    // std::println("Support Image Count: {}~{}", capabilities.minImageCount, capabilities.maxImageCount);
    // std::println("Support Array Layer: 1~{}", capabilities.maxImageArrayLayers);
    // std::println("Support Transforms: {}", vk::to_string(capabilities.supportedTransforms));
    // std::println("Support Usage Flags: {}", vk::to_string(capabilities.supportedUsageFlags));
    // std::println("Support CompositeAlpha: {}", vk::to_string(capabilities.supportedCompositeAlpha));
    // std::println("Support Extent: {}x{}~{}x{}", capabilities.minImageExtent.width,
    // capabilities.minImageExtent.height,
    //              capabilities.maxImageExtent.width, capabilities.maxImageExtent.height);
    // for (auto &supportFormat : formats)
    // {
    //     std::println("Support Format: {}", vk::to_string(supportFormat.format));
    // }
    // for (auto &supportPresentMode : presentModes)
    // {
    //     std::println("Support Present Mode: {}", vk::to_string(supportPresentMode));
    // }
    // std::println("Current Format: {}", vk::to_string(SurfaceInfo.format.format));
    // std::println("Current Color Space: {}", vk::to_string(SurfaceInfo.format.colorSpace));
    // std::println("Current Present Mode: {}", vk::to_string(SurfaceInfo.presentMode));
    // std::println("Current Extent: {}x{}", capabilities.currentExtent.width, capabilities.currentExtent.height);
    // std::println("Current Image Count: {}", SurfaceInfo.imageCount);
    // std::println("Current Image Array Layer: {}", SurfaceInfo.imageArrayLayer);
}
RHISurface::~RHISurface()
{
}
} // namespace MEngine::Platform