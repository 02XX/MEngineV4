#pragma once
#include "TextureRenderTarget.hpp"
#include "TextureRenderTargetResource.hpp"

namespace MEngine::Resource
{
using namespace MEngine::Platform;
class TextureRenderTargetVolume : public TextureRenderTarget
{

  private:
  public:
    TextureRenderTargetVolume(const std::string &name, const TextureSetting &textureSetting)
        : TextureRenderTarget(name, textureSetting)
    {
        mTextureSettings.setImageType(vk::ImageType::e3D)
            .setFlags({})
            .setArrayLayers(1); // layerCount must be 1 for 3D textures, Extent depth is used instead
        mResource = std::make_unique<TextureRenderTargetResource>(this);
    }
};
} // namespace MEngine::Resource