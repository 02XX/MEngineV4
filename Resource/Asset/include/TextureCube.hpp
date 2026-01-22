#pragma once
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"
#include <vector>

namespace MEngine::Resource
{
class TextureCube : public UploadableTexture
{

  private:
  protected:
    TextureCube() : UploadableTexture()
    {
    }

  public:
    TextureCube(const std::string &name, const TextureSetting &textureSetting, bool dynamic = false)
        : UploadableTexture(name, textureSetting, dynamic)
    {
        mTextureSettings.setImageType(vk::ImageType::e2D)
            .setArrayLayers(6)
            .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
        mResource = std::make_unique<UploadableTextureResource>(this);
    }
};
} // namespace MEngine::Resource