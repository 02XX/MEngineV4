#pragma once

#include "Math.hpp"
#include "RHIBuffer.hpp"
#include "RHIHandler.hpp"
#include "RHISemaphore.hpp"
#include "RHITexture.hpp"
#include "RHITextureView.hpp"
#include "TextureRenderTarget2D.hpp"
#include <memory>

using namespace MEngine::Platform;
namespace MEngine::Resource
{

class FrameResource
{
  public:
    // MRT
    vk::Extent3D Extent;
    std::unique_ptr<Resource::TextureRenderTarget2D> ColorTextures;
    std::unique_ptr<Resource::TextureRenderTarget2D> AlbedoTextures;
    std::unique_ptr<Resource::TextureRenderTarget2D> NormalTextures;
    std::unique_ptr<Resource::TextureRenderTarget2D> ARMTextures;
    std::unique_ptr<Resource::TextureRenderTarget2D> PositionTextures;
    std::unique_ptr<Resource::TextureRenderTarget2D> EmissiveTextures;
    std::unique_ptr<Resource::TextureRenderTarget2D> DepthStencilTextures;
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
    vk::UniqueCommandBuffer CommandBuffer;
    FrameResource(vk::Extent3D extent = {800, 600, 1});
};
} // namespace MEngine::Resource