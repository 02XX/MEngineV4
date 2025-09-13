#pragma once
#include "RHIResource.hpp"
#include <vector>
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

    vk::ImageCreateInfo mImageCreateInfo{};
    vk::ImageViewCreateInfo mImageViewCreateInfo{};
    vk::ImageType mType{vk::ImageType::e2D};
    vk::Extent3D mExtent{};
    unsigned int mMipmapLevels{1};
    unsigned int mArrayLevel{1};
    vk::Format mFormat{vk::Format::eR8G8B8A8Srgb};
    vk::ImageUsageFlags mUsages{};
    vk::SampleCountFlagBits mSampleCount{vk::SampleCountFlagBits::e1};
    vk::ImageCreateFlags mImageCreateFlags{};

  protected:
    RHITexture() : RHIResource()
    {
    }

  public:
    ~RHITexture() override = default;
    inline vk::Image GetImage() const
    {
        return mImage;
    }
};
} // namespace MEngine::Platform