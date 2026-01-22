#pragma once
#include "TextureRenderTarget.hpp"
#include "TextureRenderTargetResource.hpp"

namespace MEngine::Resource
{
using namespace MEngine::Platform;
class TextureRenderTarget2D : public TextureRenderTarget
{

  public:
  private:
  public:
    TextureRenderTarget2D(const std::string &name, const TextureSetting &textureSetting)
        : TextureRenderTarget(name, textureSetting)
    {
        mTextureSettings.setImageType(vk::ImageType::e2D).setArrayLayers(1).setFlags({});
        mResource = std::make_unique<TextureRenderTargetResource>(this);
    }
};
} // namespace MEngine::Resource