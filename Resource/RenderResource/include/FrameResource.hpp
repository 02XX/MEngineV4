#pragma once
#include "RenderResource.hpp"
#include "TextureRenderTarget2DResource.hpp"
#include <memory>

using namespace MEngine::Platform;
using namespace MEngine::Resource;
namespace MEngine::Resource
{
class FrameResource final : public RenderResource
{
  public:
    // MRT
    vk::Extent3D Extent;
    std::unique_ptr<TextureRenderTarget2DResource> ColorTextures;
    std::unique_ptr<TextureRenderTarget2DResource> AlbedoTextures;
    std::unique_ptr<TextureRenderTarget2DResource> NormalTextures;
    std::unique_ptr<TextureRenderTarget2DResource> ARMTextures;
    std::unique_ptr<TextureRenderTarget2DResource> PositionTextures;
    std::unique_ptr<TextureRenderTarget2DResource> EmissiveTextures;
    std::unique_ptr<TextureRenderTarget2DResource> DepthStencilTextures;
    vk::ClearColorValue ColorClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue AlbedoClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue NormalClearValue = {std::array<float, 4>{0.5f, 0.5f, 1.0f, 1.0f}};
    vk::ClearColorValue ARMClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue PositionClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue EmissiveClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearDepthStencilValue DepthClearValue = {1.0f, 0};
    // Semaphore
    vk::UniqueSemaphore ImageAvailableSemaphore;
    vk::UniqueSemaphore RenderFinishedSemaphore;
    // Fence
    vk::UniqueFence InFlightFence;
    vk::UniqueFence CopyFence;
    // CommandBuffer
    vk::CommandPool GraphicsCommandPool;
    vk::CommandPool TransferCommandPool;
    vk::CommandPool PresentCommandPool;

    vk::CommandBuffer GraphicsCommandBuffer;
    vk::CommandBuffer TransferCommandBuffer;
    vk::CommandBuffer PresentCommandBuffer;
    FrameResource(vk::Extent3D extent = {800, 600, 1});
    ~FrameResource() override = default;

  protected:
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource