#pragma once
#include "RHIResource.hpp"
#include "RHISwapChain.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace MEngine::Platform
{
using RHISwapChainDesc = vk::SwapchainCreateInfoKHR;
class RHISwapChain final : public RHIResource
{
  private:
    RHISwapChainDesc mDesc{};
    vk::UniqueSwapchainKHR mSwapchain{nullptr};
    std::vector<vk::Image> mSwapchainImages{};

  public:
    RHISwapChain(const RHISwapChainDesc &desc);
    ~RHISwapChain() override;
    inline const RHISwapChainDesc &GetDesc() const
    {
        return mDesc;
    }
    inline const vk::SwapchainKHR &GetSwapChain() const
    {
        return *mSwapchain;
    }
    inline const std::vector<vk::Image> &GetSwapChainImages() const
    {
        return mSwapchainImages;
    }
};
} // namespace MEngine::Platform