#pragma once
#include "TextureRenderTarget.hpp"
#include "TextureRenderTargetResource.hpp"

namespace MEngine::Resource
{
using namespace MEngine::Platform;
class TextureRenderTargetCube : public TextureRenderTarget
{

  private:
  public:
    TextureRenderTargetCube(const std::string &name, const TextureSetting &textureSetting)
        : TextureRenderTarget(name, textureSetting)
    {
        mTextureSettings.setImageType(vk::ImageType::e2D)
            .setArrayLayers(6)
            .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
        mResource = std::make_unique<TextureRenderTargetResource>(this);
    }
};
} // namespace MEngine::Resource