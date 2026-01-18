#include "OffscreenFrameResource.hpp"
#include "RenderResource.hpp"
#include "TextureRenderTarget2DResource.hpp"
#include <memory>
using namespace MEngine::Resource;
using namespace MEngine::Platform;
namespace MEngine::Function
{
OffscreenFrameResource::OffscreenFrameResource(std::shared_ptr<Context> context, vk::Extent3D extent)
    : mContext(context), Extent(extent)
{
    auto device = mContext->Device.get();
    // Semaphore
    ImageAvailableSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    RenderFinishedSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    TransferFinishedSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    // Fence
    InFlightFence = device.createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
    TransferFence = device.createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
    // CommandBuffer
    GraphicsCommandPool =
        device.createCommandPool(vk::CommandPoolCreateInfo{}
                                     .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                     .setQueueFamilyIndex(mContext->QueueFamilyIndicates.graphicsFamily.value()));
    TransferCommandPool =
        device.createCommandPool(vk::CommandPoolCreateInfo{}
                                     .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                     .setQueueFamilyIndex(mContext->QueueFamilyIndicates.transferFamily.value()));
    PresentCommandPool =
        device.createCommandPool(vk::CommandPoolCreateInfo{}
                                     .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                     .setQueueFamilyIndex(mContext->QueueFamilyIndicates.graphicsFamily.value()));
    GraphicsCommandBuffer = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo{}
                                                              .setCommandPool(GraphicsCommandPool)
                                                              .setLevel(vk::CommandBufferLevel::ePrimary)
                                                              .setCommandBufferCount(1))[0];
    TransferCommandBuffer = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo{}
                                                              .setCommandPool(TransferCommandPool)
                                                              .setLevel(vk::CommandBufferLevel::ePrimary)
                                                              .setCommandBufferCount(1))[0];
    PresentCommandBuffer = device.allocateCommandBuffers(vk::CommandBufferAllocateInfo{}
                                                             .setCommandPool(PresentCommandPool)
                                                             .setLevel(vk::CommandBufferLevel::ePrimary)
                                                             .setCommandBufferCount(1))[0];
    RecreateMRT(extent);
};
OffscreenFrameResource::~OffscreenFrameResource()
{
    ColorTexture->GetResource()->ReleaseResource(mContext);
    AlbedoTexture->GetResource()->ReleaseResource(mContext);
    NormalTexture->GetResource()->ReleaseResource(mContext);
    ARMTexture->GetResource()->ReleaseResource(mContext);
    PositionTexture->GetResource()->ReleaseResource(mContext);
    EmissiveTexture->GetResource()->ReleaseResource(mContext);
    DepthStencilTexture->GetResource()->ReleaseResource(mContext);
    auto device = mContext->Device.get();
    device.freeCommandBuffers(GraphicsCommandPool, {GraphicsCommandBuffer});
    device.freeCommandBuffers(TransferCommandPool, {TransferCommandBuffer});
    device.freeCommandBuffers(PresentCommandPool, {PresentCommandBuffer});
    device.destroyCommandPool(GraphicsCommandPool);
    device.destroyCommandPool(TransferCommandPool);
    device.destroyCommandPool(PresentCommandPool);
};
void OffscreenFrameResource::RecreateMRT(vk::Extent3D extent)
{
    Extent = extent;
    if (ColorTexture)
        ColorTexture->GetResource()->ReleaseResource(mContext);
    if (AlbedoTexture)
        AlbedoTexture->GetResource()->ReleaseResource(mContext);
    if (NormalTexture)
        NormalTexture->GetResource()->ReleaseResource(mContext);
    if (ARMTexture)
        ARMTexture->GetResource()->ReleaseResource(mContext);
    if (PositionTexture)
        PositionTexture->GetResource()->ReleaseResource(mContext);
    if (EmissiveTexture)
        EmissiveTexture->GetResource()->ReleaseResource(mContext);
    if (DepthStencilTexture)
        DepthStencilTexture->GetResource()->ReleaseResource(mContext);
    // Recreate with new extent
    // MRT
    // Color Attachment
    vk::ImageCreateInfo colorImageCreateInfo{};
    colorImageCreateInfo.setImageType(vk::ImageType::e2D)
        .setFormat(vk::Format::eR32G32B32A32Sfloat)
        .setExtent(Extent)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst |
                  vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);
    vk::SamplerCreateInfo colorImageSamplerCreateInfo{};
    colorImageSamplerCreateInfo.setMagFilter(vk::Filter::eLinear)
        .setMinFilter(vk::Filter::eLinear)
        .setAddressModeU(vk::SamplerAddressMode::eRepeat)
        .setAddressModeV(vk::SamplerAddressMode::eRepeat)
        .setAddressModeW(vk::SamplerAddressMode::eRepeat)
        // .setAnisotropyEnable(VK_TRUE)
        // .setMaxAnisotropy(16)
        .setBorderColor(vk::BorderColor::eIntOpaqueBlack)
        .setUnnormalizedCoordinates(VK_FALSE)
        .setCompareEnable(VK_FALSE)
        .setCompareOp(vk::CompareOp::eAlways)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear)
        .setMipLodBias(0.0f)
        .setMinLod(0.0f)
        .setMaxLod(0.0f);
    ColorTexture = std::make_unique<TextureRenderTarget2D>("Color0", colorImageCreateInfo, colorImageSamplerCreateInfo);
    AlbedoTexture =
        std::make_unique<TextureRenderTarget2D>("Albedo1", colorImageCreateInfo, colorImageSamplerCreateInfo);
    NormalTexture =
        std::make_unique<TextureRenderTarget2D>("Normal2", colorImageCreateInfo, colorImageSamplerCreateInfo);
    ARMTexture = std::make_unique<TextureRenderTarget2D>("ARM3", colorImageCreateInfo, colorImageSamplerCreateInfo);
    PositionTexture =
        std::make_unique<TextureRenderTarget2D>("Position4", colorImageCreateInfo, colorImageSamplerCreateInfo);
    EmissiveTexture =
        std::make_unique<TextureRenderTarget2D>("Emissive5", colorImageCreateInfo, colorImageSamplerCreateInfo);
    vk::ImageCreateInfo depthStencilImageCreateInfo{};
    depthStencilImageCreateInfo.setImageType(vk::ImageType::e2D)
        .setFormat(vk::Format::eD32SfloatS8Uint)
        .setExtent(Extent)
        .setMipLevels(1)
        .setArrayLayers(1)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setTiling(vk::ImageTiling::eOptimal)
        .setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst |
                  vk::ImageUsageFlagBits::eTransferSrc)
        .setSharingMode(vk::SharingMode::eExclusive)
        .setInitialLayout(vk::ImageLayout::eUndefined);
    vk::SamplerCreateInfo depthStencilImageSamplerCreateInfo{};
    DepthStencilTexture = std::make_unique<TextureRenderTarget2D>("DepthStencil6", depthStencilImageCreateInfo,
                                                                  colorImageSamplerCreateInfo);
    // Init RHI
    ColorTexture->GetResource()->InitResource(mContext);
    AlbedoTexture->GetResource()->InitResource(mContext);
    NormalTexture->GetResource()->InitResource(mContext);
    ARMTexture->GetResource()->InitResource(mContext);
    PositionTexture->GetResource()->InitResource(mContext);
    EmissiveTexture->GetResource()->InitResource(mContext);
    DepthStencilTexture->GetResource()->InitResource(mContext);

    mContext->Device->waitIdle();
    vk::ImageMemoryBarrier2 colorBarrier{}, depthBarrier{}, albedoBarrier{}, normalBarrier{}, armBarrier{},
        positionBarrier{}, emissiveBarrier{};
    colorBarrier.setImage(ColorTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    albedoBarrier.setImage(AlbedoTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    normalBarrier.setImage(NormalTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    armBarrier.setImage(ARMTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    positionBarrier.setImage(PositionTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    emissiveBarrier.setImage(EmissiveTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});
    depthBarrier.setImage(DepthStencilTexture->GetResourceAs<TextureRenderTarget2DResource>()->GetImage())
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentWrite)
        .setSubresourceRange({vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1});
    std::vector<vk::ImageMemoryBarrier2> barriers = {colorBarrier,    albedoBarrier,   normalBarrier, armBarrier,
                                                     positionBarrier, emissiveBarrier, depthBarrier};
    TransferCommandBuffer.begin(vk::CommandBufferBeginInfo{});
    TransferCommandBuffer.pipelineBarrier2(vk::DependencyInfo().setImageMemoryBarriers(barriers));
    TransferCommandBuffer.end();
    vk::SubmitInfo2 submitInfo{};
    std::vector<vk::CommandBufferSubmitInfo> commandBufferInfos = {
        vk::CommandBufferSubmitInfo().setCommandBuffer(TransferCommandBuffer),
    };
    submitInfo.setCommandBufferInfos(commandBufferInfos);
    mContext->TransferQueue.submit2(submitInfo, {});
    mContext->Device->waitIdle();
}
} // namespace MEngine::Function