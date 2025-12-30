#include "SwapChainResource.hpp"
#include "Logger.hpp"
#include "RHIContext.hpp"
#include "RHITexture.hpp"
#include "RHITextureView.hpp"
using namespace MEngine::Platform;
namespace MEngine::Resource
{
void SwapChainResource::InitRHI()
{
    auto &rhiContext = RHIContext::Instance();
    auto surfaceInfo = rhiContext.GetSurfaceInfo();
    RHISwapChainDesc swapChainDesc{};
    swapChainDesc.setOldSwapchain(nullptr)
        .setSurface(rhiContext.GetSurface())
        .setMinImageCount(surfaceInfo.imageCount)
        .setImageFormat(surfaceInfo.format.format)
        .setImageColorSpace(surfaceInfo.format.colorSpace)
        .setImageExtent(surfaceInfo.extent)
        .setImageArrayLayers(surfaceInfo.imageArrayLayer)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst |
                       vk::ImageUsageFlagBits::eTransferSrc)
        .setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(surfaceInfo.presentMode)
        .setClipped(vk::True);
    mRHISwapChainHandler = RHIHandler<RHISwapChain>(new RHISwapChain(swapChainDesc));
    auto swapChainImages = mRHISwapChainHandler->GetSwapChainImages();
    mSwapChainTextures.resize(swapChainImages.size());
    mSwapChainTextureViews.resize(swapChainImages.size());
    vk::ImageSubresourceRange subResourceRange{};
    subResourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseArrayLayer(0)
        .setLevelCount(1)
        .setLayerCount(1)
        .setBaseMipLevel(0);
    for (size_t i = 0; i < swapChainImages.size(); ++i)
    {
        mSwapChainTextures[i] = RHIHandler<RHITexture>(new RHITexture(swapChainImages[i], subResourceRange));
        mSwapChainTextures[i]->TransitionImageLayout(vk::ImageLayout::ePresentSrcKHR);
        RHITextureViewDesc viewDesc{};
        viewDesc.image = mSwapChainTextures[i]->GetImage();
        viewDesc.viewType = vk::ImageViewType::e2D;
        viewDesc.format = surfaceInfo.format.format;
        viewDesc.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        viewDesc.subresourceRange.baseMipLevel = 0;
        viewDesc.subresourceRange.levelCount = 1;
        viewDesc.subresourceRange.baseArrayLayer = 0;
        viewDesc.subresourceRange.layerCount = 1;
        mSwapChainTextureViews[i] = RHIHandler<RHITextureView>(new RHITextureView(viewDesc));
    }
}
void SwapChainResource::ReleaseRHI()
{
    if (mRHISwapChainHandler)
    {
        mRHISwapChainHandler.SafeRelease();
    }
    for (auto &textureView : mSwapChainTextureViews)
    {
        if (textureView)
        {
            textureView.SafeRelease();
        }
    }
    mSwapChainTextureViews.clear();
    for (auto &texture : mSwapChainTextures)
    {
        if (texture)
        {
            texture.SafeRelease();
        }
    }
    mSwapChainTextures.clear();
}
void SwapChainResource::Present(FrameResource &frameResource)
{
    auto &rhiContext = RHIContext::Instance();
    uint32_t imageIndex = 0;
    auto result = rhiContext.GetDevice().acquireNextImageKHR(
        mRHISwapChainHandler->GetSwapChain(), std::numeric_limits<uint64_t>::max(), nullptr, nullptr, &imageIndex);
    if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
    {
        LogError("Failed to acquire swap chain image!");
        return;
    }
    auto commandBuffer = frameResource.CommandBuffer.get();
    commandBuffer.begin(vk::CommandBufferBeginInfo{});
    auto swapChainImage = mSwapChainTextures[imageIndex];
    swapChainImage->TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal);
    auto colorAttachment = frameResource.ColorTextures->GetResourceAs<TextureRenderTarget2DResource>()->GetTexture();
    colorAttachment->CopyTo(swapChainImage.Get());
    swapChainImage->TransitionImageLayout(vk::ImageLayout::ePresentSrcKHR);
    commandBuffer.end();
    vk::SubmitInfo submitinfo;
    std::vector<vk::PipelineStageFlags> waitStages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitinfo.setCommandBuffers(commandBuffer)
        .setSignalSemaphores(frameResource.RenderFinishedSemaphore.get())
        .setWaitSemaphores(frameResource.ImageAvailableSemaphore.get())
        .setWaitDstStageMask(waitStages);
    rhiContext.GetTransferQueue().submit({submitinfo}, nullptr);

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setSwapchains(mRHISwapChainHandler->GetSwapChain())
        .setImageIndices(imageIndex)
        .setWaitSemaphores({frameResource.RenderFinishedSemaphore.get()});
    result = rhiContext.GetPresentQueue().presentKHR({presentInfo});
    if (result != vk::Result::eSuccess)
    {
        LogError("Failed to present swap chain image!");
        return;
    }
}
} // namespace MEngine::Resource