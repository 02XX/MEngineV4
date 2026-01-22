#pragma once
#include "TextureResource.hpp"
namespace MEngine::Resource
{
class TextureRenderTarget;
class TextureRenderTargetResource : public TextureResource
{

  public:
    TextureRenderTargetResource(TextureRenderTarget *textureRenderTarget);
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource