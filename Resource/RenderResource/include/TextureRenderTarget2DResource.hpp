#pragma once

#include "RHIHandler.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "RHITextureView.hpp"
#include "RenderResource.hpp"
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class TextureRenderTarget2D;
class TextureRenderTarget2DResource : public RenderResource
{
  private:
    RHIHandler<RHITexture> mTexture{};
    RHIHandler<RHISampler> mSampler{};
    RHIHandler<RHITextureView> mTextureView{};
    TextureRenderTarget2D *mTextureRenderTarget2D{};

  public:
    TextureRenderTarget2DResource(TextureRenderTarget2D *texture) : RenderResource(), mTextureRenderTarget2D(texture)
    {
    }
    virtual void InitRHI() override;
    virtual void ReleaseRHI() override;
    inline RHIHandler<RHITexture> GetTexture() const
    {
        return mTexture;
    }
    inline RHIHandler<RHISampler> GetSampler() const
    {
        return mSampler;
    }
    inline RHIHandler<RHITextureView> GetTextureView() const
    {
        return mTextureView;
    }
};
} // namespace MEngine::Resource