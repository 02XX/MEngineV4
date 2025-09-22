#include "RHISwapChain.hpp"
#include "RHIContext.hpp"

namespace MEngine::Platform
{
RHISwapChain::RHISwapChain(const RHISwapChainDesc &desc) : mDesc(desc)
{
    auto queueFamilyIndicates = RHIContext::Instance().GetQueueFamilyIndicates();
    if (queueFamilyIndicates.graphicsFamily == queueFamilyIndicates.presentFamily)
    {
        mDesc.setImageSharingMode(vk::SharingMode::eExclusive)
            .setQueueFamilyIndices({queueFamilyIndicates.graphicsFamily.value()});
    }
    else
    {
        std::array<uint32_t, 2> queueFamilyIndicesArray = {queueFamilyIndicates.graphicsFamily.value(),
                                                           queueFamilyIndicates.presentFamily.value()};
        mDesc.setImageSharingMode(vk::SharingMode::eConcurrent).setQueueFamilyIndices(queueFamilyIndicesArray);
    }

    mSwapchain = RHIContext::Instance().GetDevice().createSwapchainKHRUnique(mDesc);
    if (!mSwapchain)
    {
        throw std::runtime_error("Failed to create swapchain");
    }
    auto swapchainImages = RHIContext::Instance().GetDevice().getSwapchainImagesKHR(mSwapchain.get());
    for (auto &image : swapchainImages)
    {
        mSwapchainImages.push_back(image);
    }
}
RHISwapChain::~RHISwapChain()
{
}
} // namespace MEngine::Platform