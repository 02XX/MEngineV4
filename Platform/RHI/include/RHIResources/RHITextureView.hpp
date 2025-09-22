#pragma once

#include "RHIResource.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
using RHITextureViewDesc = vk::ImageViewCreateInfo;
class RHITextureView : public RHIResource
{
  private:
    vk::UniqueImageView mImageView{};
    RHITextureViewDesc mTextureViewDesc{};

  protected:
  public:
    RHITextureView(const RHITextureViewDesc &createInfo);
    ~RHITextureView() override = default;
    inline vk::ImageView GetImageView() const
    {
        return mImageView.get();
    }
};
} // namespace MEngine::Platform