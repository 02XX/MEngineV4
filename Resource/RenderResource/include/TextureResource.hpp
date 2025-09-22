#pragma once
#include "RHIHandler.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "RHITextureView.hpp"
#include "RenderResource.hpp"
#include "Texture.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class TextureResource : public RenderResource
{
  protected:
    RHIHandler<RHITexture> mRHITextureHandler;
    RHIHandler<RHISampler> mRHISamplerHandler;
    RHIHandler<RHITextureView> mRHITextureViewHandler;

  public:
    TextureResource() : RenderResource()
    {
    }
    TextureResource(RHIHandler<RHITexture> texture, RHIHandler<RHITextureView> textureView)
        : RenderResource(), mRHITextureHandler(texture), mRHITextureViewHandler(textureView)
    {
    }
    inline RHIHandler<RHITexture> GetTexture() const
    {
        return mRHITextureHandler;
    }
    inline RHIHandler<RHISampler> GetSampler() const
    {
        return mRHISamplerHandler;
    }
    inline RHIHandler<RHITextureView> GetTextureView() const
    {
        return mRHITextureViewHandler;
    }
    std::pair<uint32_t, uint32_t> GetPixelSize(vk::Format format) const;
};
} // namespace MEngine::Resource
