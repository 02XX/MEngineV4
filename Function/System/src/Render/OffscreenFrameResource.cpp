#include "OffscreenFrameResource.hpp"
#include "RenderResource.hpp"
#include "Texture.hpp"
#include <memory>
#include <vulkan/vulkan_structs.hpp>
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
    TransferFence = device.createFenceUnique(vk::FenceCreateInfo());
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
    // Recreate with new extent
    // MRT
    // Color Attachment
    TextureSetting textureSetting{};
    textureSetting.setImageType(vk::ImageType::e2D)
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
    textureSetting.setMagFilter(vk::Filter::eLinear)
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
    AlbedoTexture = std::make_unique<TextureRenderTarget2D>("Color0", textureSetting);
    NormalTexture = std::make_unique<TextureRenderTarget2D>("Color1", textureSetting);
    ARMTexture = std::make_unique<TextureRenderTarget2D>("Color2", textureSetting);
    PositionTexture = std::make_unique<TextureRenderTarget2D>("Color3", textureSetting);
    EmissiveTexture = std::make_unique<TextureRenderTarget2D>("Color4", textureSetting);
    TextureSetting depthStencilTextureSetting{};
    depthStencilTextureSetting.setImageType(vk::ImageType::e2D)
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
    DepthStencilTexture = std::make_unique<TextureRenderTarget2D>("DepthStencil5", depthStencilTextureSetting);
    // Init RHI
    // AlbedoTexture->PendingInit();
    // NormalTexture->PendingInit();
    // ARMTexture->PendingInit();
    // PositionTexture->PendingInit();
    // EmissiveTexture->PendingInit();
    // DepthStencilTexture->PendingInit();
    AlbedoTexture->GetResource()->InitResource(mContext);
    NormalTexture->GetResource()->InitResource(mContext);
    ARMTexture->GetResource()->InitResource(mContext);
    PositionTexture->GetResource()->InitResource(mContext);
    EmissiveTexture->GetResource()->InitResource(mContext);
    DepthStencilTexture->GetResource()->InitResource(mContext);

    mContext->Device->waitIdle();
    std::vector<vk::ImageMemoryBarrier2> barriers{};
    vk::ImageSubresourceRange subresourceRange{};
    subresourceRange.setBaseMipLevel(0).setBaseArrayLayer(0);
    vk::ImageMemoryBarrier2 barrier{};
    barrier.setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
        .setOldLayout(vk::ImageLayout::eUndefined)
        .setNewLayout(vk::ImageLayout::eColorAttachmentOptimal)
        .setSrcStageMask(vk::PipelineStageFlagBits2::eTopOfPipe)
        .setDstStageMask(vk::PipelineStageFlagBits2::eColorAttachmentOutput)
        .setSrcAccessMask(vk::AccessFlagBits2::eNone)
        .setDstAccessMask(vk::AccessFlagBits2::eColorAttachmentWrite | vk::AccessFlagBits2::eColorAttachmentRead);
    // Albedo
    subresourceRange.setLevelCount(1).setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.setImage(AlbedoTexture->GetResourceAs<TextureResource>()->mImage).setSubresourceRange(subresourceRange);
    barriers.push_back(barrier);
    // Normal
    barrier.setImage(NormalTexture->GetResourceAs<TextureResource>()->mImage).setSubresourceRange(subresourceRange);
    barriers.push_back(barrier);
    // ARM
    barrier.setImage(ARMTexture->GetResourceAs<TextureResource>()->mImage).setSubresourceRange(subresourceRange);
    barriers.push_back(barrier);
    // Position
    barrier.setImage(PositionTexture->GetResourceAs<TextureResource>()->mImage).setSubresourceRange(subresourceRange);
    barriers.push_back(barrier);
    // Emissive
    barrier.setImage(EmissiveTexture->GetResourceAs<TextureResource>()->mImage).setSubresourceRange(subresourceRange);
    barriers.push_back(barrier);
    // DepthStencil
    subresourceRange.setLevelCount(1).setLayerCount(1).setAspectMask(vk::ImageAspectFlagBits::eDepth |
                                                                     vk::ImageAspectFlagBits::eStencil);
    barrier.setImage(DepthStencilTexture->GetResourceAs<TextureResource>()->mImage)
        .setSubresourceRange(subresourceRange)
        .setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
        .setDstStageMask(vk::PipelineStageFlagBits2::eEarlyFragmentTests |
                         vk::PipelineStageFlagBits2::eLateFragmentTests)
        .setDstAccessMask(vk::AccessFlagBits2::eDepthStencilAttachmentRead |
                          vk::AccessFlagBits2::eDepthStencilAttachmentWrite);
    barriers.push_back(barrier);
    vk::DependencyInfo dependencyInfo{};
    dependencyInfo.setImageMemoryBarriers(barriers);
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    TransferCommandBuffer.begin(beginInfo);
    TransferCommandBuffer.pipelineBarrier2(dependencyInfo);
    TransferCommandBuffer.end();
    vk::SubmitInfo2 submitInfo{};
    submitInfo.setCommandBufferInfos(
        vk::CommandBufferSubmitInfo{}.setCommandBuffer(TransferCommandBuffer).setDeviceMask(0));
    mContext->GraphicsQueue.submit2({submitInfo}, {});
    mContext->Device->waitIdle();
}
} // namespace MEngine::Function