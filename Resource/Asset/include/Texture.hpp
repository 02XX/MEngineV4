#pragma once
#include "Asset.hpp"
#include "UUID.hpp"
#include <filesystem>
#include <gtest/gtest_prod.h>
#include <nlohmann/json_fwd.hpp>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
namespace MEngine::Resource
{
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Engine/UTexture
class Texture : public Asset
{
  private:
  protected:
    std::vector<uint8_t> mImageData{};
    vk::Image mImage{};
    vk::UniqueImageView mImageView{};
    VmaAllocation mAllocation{};
    VmaAllocationInfo mAllocationInfo{};
    // image
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
    Texture() : Asset()
    {
    }

  public:
    ~Texture() override = default;
    inline vk::Image GetImage() const
    {
        return mImage;
    }
    inline vk::ImageView GetImageView() const
    {
        return mImageView.get();
    }
};
} // namespace MEngine::Resource