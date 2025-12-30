#include "FrameResource.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "SceneResource.hpp"
#include <memory>
using namespace MEngine::Resource;
using namespace MEngine::Platform;
namespace MEngine::Resource
{
FrameResource::FrameResource(vk::Extent3D extent) : Extent(extent)
{
    auto &device = Platform::RHIContext::Instance().GetDevice();
    // Semaphore
    ImageAvailableSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    RenderFinishedSemaphore = device.createSemaphoreUnique(vk::SemaphoreCreateInfo{});
    // Fence
    InFlightFence = device.createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
    CopyFence = device.createFenceUnique(vk::FenceCreateInfo().setFlags(vk::FenceCreateFlagBits::eSignaled));
    // CommandBuffer
    CommandBuffer = RHIContext::Instance().GetGraphicsCommandBuffer();
    // MRT
    TextureSetting colorSetting{};
    colorSetting.imageType = vk::ImageType::e2D;
    colorSetting.format = vk::Format::eR8G8B8A8Srgb;
    colorSetting.extent = Extent;
    colorSetting.mipLevels = 1;
    colorSetting.arrayLayers = 1;
    colorSetting.samples = vk::SampleCountFlagBits::e1;
    colorSetting.tiling = vk::ImageTiling::eOptimal;
    colorSetting.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst |
                         vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled;
    colorSetting.sharingMode = vk::SharingMode::eExclusive;
    RHISamplerDesc samplerDesc{};
    ColorTextures = std::make_unique<TextureRenderTarget2D>("Color0", colorSetting, samplerDesc);
    colorSetting.format = vk::Format::eR32G32B32A32Sfloat;
    AlbedoTextures = std::make_unique<TextureRenderTarget2D>("Albedo1", colorSetting, samplerDesc);
    NormalTextures = std::make_unique<TextureRenderTarget2D>("Normal2", colorSetting, samplerDesc);
    ARMTextures = std::make_unique<TextureRenderTarget2D>("ARM3", colorSetting, samplerDesc);
    PositionTextures = std::make_unique<TextureRenderTarget2D>("Position4", colorSetting, samplerDesc);
    EmissiveTextures = std::make_unique<TextureRenderTarget2D>("Emissive5", colorSetting, samplerDesc);
    ColorTextures->GetResource()->InitRHI();
    AlbedoTextures->GetResource()->InitRHI();
    NormalTextures->GetResource()->InitRHI();
    ARMTextures->GetResource()->InitRHI();
    PositionTextures->GetResource()->InitRHI();
    EmissiveTextures->GetResource()->InitRHI();
    // DepthStencil
    TextureSetting depthStencilSetting{};
    depthStencilSetting.extent = Extent;
    depthStencilSetting.mipLevels = 1;
    depthStencilSetting.format = vk::Format::eD32SfloatS8Uint;
    depthStencilSetting.samples = vk::SampleCountFlagBits::e1;
    DepthStencilTextures = std::make_unique<TextureRenderTarget2D>("DepthStencil6", depthStencilSetting, samplerDesc);
    DepthStencilTextures->GetResource()->InitRHI();
};
} // namespace MEngine::Resource