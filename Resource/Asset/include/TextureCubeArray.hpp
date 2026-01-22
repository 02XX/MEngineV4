#pragma once
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"
#include <vector>

namespace MEngine::Resource
{
class TextureCubeArray : public UploadableTexture
{

  protected:
    TextureCubeArray() : UploadableTexture()
    {
    }

  public:
    TextureCubeArray(const std::string &name, const TextureSetting &textureSetting, bool dynamic = false)
        : UploadableTexture(name, textureSetting, dynamic)
    {
        mTextureSettings.setImageType(vk::ImageType::e2D)
            .setArrayLayers(12)
            .setFlags(vk::ImageCreateFlagBits::eCubeCompatible);
        mResource = std::make_unique<UploadableTextureResource>(this);
    }
};
} // namespace MEngine::Resource