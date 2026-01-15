#pragma once
#include "TextureResource.hpp"
namespace MEngine::Resource
{
class TextureRenderTarget2D;
class TextureRenderTarget2DResource : public TextureResource
{

  public:
    TextureRenderTarget2DResource(TextureRenderTarget2D *textureRenderTarget2D);
    virtual void InitRHI(std::shared_ptr<Context> context) override;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource