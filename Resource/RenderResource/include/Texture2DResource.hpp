
#pragma once
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "TextureResource.hpp"
#include <cstdint>

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Texture2D;
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
};
} // namespace MEngine::Resource
