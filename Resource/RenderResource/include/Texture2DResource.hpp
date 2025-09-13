
#pragma once
#include "RHIHandler.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "RenderResource.hpp"
#include "Texture2D.hpp"
#include "TextureResource.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Texture2DResource : public TextureResource
{
  protected:
    Texture2D *mTexture{nullptr};

  public:
    Texture2DResource(Texture2D *texture) : TextureResource(), mTexture(texture)
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
