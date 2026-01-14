#include "FrameResource.hpp"
#include "RenderResource.hpp"
#include "TextureRenderTarget2DResource.hpp"
#include <memory>
using namespace MEngine::Resource;
using namespace MEngine::Platform;
namespace MEngine::Resource
{
FrameResource::FrameResource(vk::Extent3D extent) : Extent(extent) {};
void FrameResource::InitRHI(std::shared_ptr<Context> context)
{

    auto device = context->Device.get();
    // Semaphore
    ImageAvailableSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    RenderFinishedSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    // Fence
    InFlightFence = device.createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
    CopyFence = device.createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
    // CommandBuffer
    GraphicsCommandPool =
        device.createCommandPool(vk::CommandPoolCreateInfo{}
                                     .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                     .setQueueFamilyIndex(context->QueueFamilyIndicates.graphicsFamily.value()));
    TransferCommandPool =
        device.createCommandPool(vk::CommandPoolCreateInfo{}
                                     .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                     .setQueueFamilyIndex(context->QueueFamilyIndicates.transferFamily.value()));
    PresentCommandPool =
        device.createCommandPool(vk::CommandPoolCreateInfo{}
                                     .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                                     .setQueueFamilyIndex(context->QueueFamilyIndicates.graphicsFamily.value()));
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
    // MRT
    // Color Attachment
    vk::ImageCreateInfo colorImageCreateInfo{};
    colorImageCreateInfo.setImageType(vk::ImageType::e2D)
        .setFormat(vk::Format::eR8G8B8A8Srgb)
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
    ColorTexture = std::make_unique<TextureRenderTarget2DResource>(colorImageCreateInfo, colorImageSamplerCreateInfo);
    AlbedoTexture = std::make_unique<TextureRenderTarget2DResource>(colorImageCreateInfo, colorImageSamplerCreateInfo);
    NormalTexture = std::make_unique<TextureRenderTarget2DResource>(colorImageCreateInfo, colorImageSamplerCreateInfo);
    ARMTexture = std::make_unique<TextureRenderTarget2DResource>(colorImageCreateInfo, colorImageSamplerCreateInfo);
    PositionTexture =
        std::make_unique<TextureRenderTarget2DResource>(colorImageCreateInfo, colorImageSamplerCreateInfo);
    EmissiveTexture =
        std::make_unique<TextureRenderTarget2DResource>(colorImageCreateInfo, colorImageSamplerCreateInfo);

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
    DepthStencilTexture =
        std::make_unique<TextureRenderTarget2DResource>(depthStencilImageCreateInfo, colorImageSamplerCreateInfo);
    // Init RHI
    ColorTexture->InitRHI(context);
    AlbedoTexture->InitRHI(context);
    NormalTexture->InitRHI(context);
    ARMTexture->InitRHI(context);
    PositionTexture->InitRHI(context);
    EmissiveTexture->InitRHI(context);
    DepthStencilTexture->InitRHI(context);
}
void FrameResource::ReleaseRHI(std::shared_ptr<Context> context)
{

    ColorTexture->ReleaseRHI(context);
    AlbedoTexture->ReleaseRHI(context);
    NormalTexture->ReleaseRHI(context);
    ARMTexture->ReleaseRHI(context);
    PositionTexture->ReleaseRHI(context);
    EmissiveTexture->ReleaseRHI(context);
    DepthStencilTexture->ReleaseRHI(context);
    auto device = context->Device.get();
    device.freeCommandBuffers(GraphicsCommandPool, {GraphicsCommandBuffer});
    device.freeCommandBuffers(TransferCommandPool, {TransferCommandBuffer});
    device.freeCommandBuffers(PresentCommandPool, {PresentCommandBuffer});
    device.destroyCommandPool(GraphicsCommandPool);
    device.destroyCommandPool(TransferCommandPool);
    device.destroyCommandPool(PresentCommandPool);
}
} // namespace MEngine::Resource