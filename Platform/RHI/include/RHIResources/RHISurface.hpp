#pragma once
#include "RHIResource.hpp"
#include "RHISurface.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace MEngine::Platform
{

class RHISurface final : public RHIResource
{
  private:
    vk::UniqueSurfaceKHR mSurface{nullptr};
    struct SurfaceInfo
    {
        vk::SurfaceFormatKHR format{};
        vk::Extent2D extent{};
        vk::PresentModeKHR presentMode{};
        uint32_t imageCount{};
        uint32_t imageArrayLayer{};
    } SurfaceInfo;

  public:
    RHISurface(VkSurfaceKHR &&surface);
    RHISurface(vk::SurfaceKHR &&surface);
    ~RHISurface() override;
};
} // namespace MEngine::Platform