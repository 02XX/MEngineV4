#pragma once
#include "RHIHandler.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
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

  public:
    TextureResource() : RenderResource()
    {
    }
    virtual void InitRHI() override;
    virtual void ReleaseRHI() override;
    inline RHIHandler<RHITexture> GetTexture() const
    {
        return mRHITextureHandler;
    }
    inline RHIHandler<RHISampler> GetSampler() const
    {
        return mRHISamplerHandler;
    }
};
} // namespace MEngine::Resource
