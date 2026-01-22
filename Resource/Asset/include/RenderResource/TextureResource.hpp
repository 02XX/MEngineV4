#pragma once
#include "RenderResource.hpp"

namespace MEngine::Resource
{
class Texture;
class TextureResource : public RenderResource
{
  public:
    vk::Image mImage{};
    vk::ImageView mImageView{};
    vk::Sampler mSampler{};
    VmaAllocation mImageAllocation{};
    VmaAllocationInfo mImageAllocationInfo{};

  public:
    TextureResource(Texture *texture);
    ~TextureResource() override;
    void InitRHI(std::shared_ptr<Context> context) override = 0;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    std::pair<uint32_t, uint32_t> GetPixelSize(vk::Format format) const;
};
} // namespace MEngine::Resource
