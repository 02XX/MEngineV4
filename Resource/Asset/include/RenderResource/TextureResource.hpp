#pragma once
#include "RenderResource.hpp"

namespace MEngine::Resource
{
class Texture;
class TextureResource : public RenderResource
{
  protected:
    vk::Image mImage{};
    vk::ImageView mImageView{};
    vk::Sampler mSampler{};

    VmaAllocation mImageAllocation{};
    VmaAllocationInfo mImageAllocationInfo{};

  public:
    TextureResource(Texture *texture);
    ~TextureResource() override;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) override;
    inline const vk::Image GetImage() const
    {
        return mImage;
    }
    inline const vk::Sampler GetSampler() const
    {
        return mSampler;
    }
    const vk::ImageView GetImageView() const
    {
        return mImageView;
    }

    std::pair<uint32_t, uint32_t> GetPixelSize(vk::Format format) const;
};
} // namespace MEngine::Resource
