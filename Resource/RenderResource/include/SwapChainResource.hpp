#pragma once

#include "RenderResource.hpp"
#include <vector>

namespace MEngine::Resource
{
struct SurfaceInfo
{
    vk::SurfaceFormatKHR SurfaceFormat{};
    static constexpr std::array<vk::SurfaceFormatKHR, 6> candidatesFormats = {{
        {vk::Format::eR32G32B32A32Sfloat, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eB8G8R8A8Srgb, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
    }};
    vk::PresentModeKHR PresentMode{};
    static constexpr std::array<vk::PresentModeKHR, 4> candidatePresentModes = {
        vk::PresentModeKHR::eMailbox, vk::PresentModeKHR::eFifoRelaxed, vk::PresentModeKHR::eImmediate,
        vk::PresentModeKHR::eFifo};
    vk::SurfaceCapabilitiesKHR Capabilities{};
};

class SwapChainResource : public RenderResource
{

  private:
  public:
    HINSTANCE HInstance;
    HWND Hwnd;
    vk::SurfaceKHR Surface{};
    SurfaceInfo SurfaceInfo;

    vk::SwapchainKHR SwapChain{};
    std::vector<vk::Image> SwapChainImages{};
    std::vector<vk::ImageView> SwapChainImageViews{};

    SwapChainResource(vk::SurfaceKHR surface) : Surface(surface) {};
    ~SwapChainResource() override = default;

  protected:
    void CreateSurface(std::shared_ptr<Context> context);
    void QuerySurfaceSupport(std::shared_ptr<Context> context);
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource