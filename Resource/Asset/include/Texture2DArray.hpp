#pragma once
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"
#include <vector>

namespace MEngine::Resource
{
class Texture2DArray : public UploadableTexture
{

  protected:
    Texture2DArray() : UploadableTexture()
    {
    }

  public:
    Texture2DArray(const std::string &name, const TextureSetting &textureSetting, bool dynamic = false)
        : UploadableTexture(name, textureSetting, dynamic)
    {
        mTextureSettings.setImageType(vk::ImageType::e2D).setFlags({});
        mResource = std::make_unique<UploadableTextureResource>(this);
    }
};
} // namespace MEngine::Resource