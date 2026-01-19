#pragma once
#include "Math.hpp"
#include "TextureRenderTarget2D.hpp"
#include <cstddef>
#include <memory>
#include <vector>

using namespace MEngine::Platform;
using namespace MEngine::Resource;
namespace MEngine::Function
{
class OffscreenFrameResource
{
  protected:
    std::shared_ptr<Context> mContext;

  public:
    // MRT
    vk::Extent3D Extent;

    std::unique_ptr<TextureRenderTarget2D> ColorTexture;
    std::unique_ptr<TextureRenderTarget2D> AlbedoTexture;
    std::unique_ptr<TextureRenderTarget2D> NormalTexture;
    std::unique_ptr<TextureRenderTarget2D> ARMTexture;
    std::unique_ptr<TextureRenderTarget2D> PositionTexture;
    std::unique_ptr<TextureRenderTarget2D> EmissiveTexture;
    std::unique_ptr<TextureRenderTarget2D> DepthStencilTexture;

    vk::ClearColorValue ColorClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue AlbedoClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue NormalClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue ARMClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue PositionClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearColorValue EmissiveClearValue = {std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f}};
    vk::ClearDepthStencilValue DepthClearValue = {1.0f, 0};
    // Semaphore
    vk::UniqueSemaphore ImageAvailableSemaphore;
    vk::UniqueSemaphore RenderFinishedSemaphore;
    vk::UniqueSemaphore TransferFinishedSemaphore;
    // Fence
    vk::UniqueFence InFlightFence;
    vk::UniqueFence TransferFence;
    // CommandBuffer
    vk::CommandPool GraphicsCommandPool;
    vk::CommandPool TransferCommandPool;
    vk::CommandPool PresentCommandPool;
    // Primary CommandBuffer
    vk::CommandBuffer GraphicsCommandBuffer; // 可以派出次级命令缓冲区，然后汇总到这个缓冲区中执行
    vk::CommandBuffer TransferCommandBuffer;
    vk::CommandBuffer PresentCommandBuffer;
    std::vector<vk::CommandBuffer> SecondaryTransferCommandBuffers;
    std::vector<vk::CommandBuffer> SecondaryGraphicCommandBuffers;
    std::vector<vk::CommandBuffer> SecondaryPresentCommandBuffers;
    OffscreenFrameResource(std::shared_ptr<Context> context, vk::Extent3D extent = {800, 600, 1});
    virtual ~OffscreenFrameResource();
    void RecreateMRT(vk::Extent3D extent);
};
} // namespace MEngine::Function