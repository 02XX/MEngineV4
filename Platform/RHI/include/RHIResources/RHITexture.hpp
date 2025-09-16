#pragma once
#include "RHIContext.hpp"
#include "RHIResource.hpp"
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace MEngine::Resource
{
class Texture2DResource;
class Texture2DArrayResource;
class Texture3DResource;
} // namespace MEngine::Resource
namespace MEngine::Platform
{
using RHITextureDesc = vk::ImageCreateInfo;
struct MipMapData
{
};
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Engine/UTexture
class RHITexture : public RHIResource
{
    friend class MEngine::Resource::Texture2DResource;
    friend class MEngine::Resource::Texture2DArrayResource;
    friend class MEngine::Resource::Texture3DResource;

  private:
  protected:
    // image
    vk::Image mImage{};
    VmaAllocationCreateInfo mAllocationCreateInfo{};
    VmaAllocation mAllocation{};
    VmaAllocationInfo mAllocationInfo{};
    RHITextureDesc mTextureDesc{};
    vk::ImageLayout mCurrentLayout{vk::ImageLayout::eUndefined};

  protected:
    RHITexture(const RHITextureDesc &desc);
    void TransitionImageLayout(vk::ImageLayout newLayout);

  public:
    ~RHITexture() override;
    inline vk::Image GetImage() const
    {
        return mImage;
    }
    inline vk::ImageLayout GetCurrentLayout() const
    {
        return mCurrentLayout;
    }
    inline const RHITextureDesc &GetTextureDesc() const
    {
        return mTextureDesc;
    }
    void SetImageData();
};
} // namespace MEngine::Platform