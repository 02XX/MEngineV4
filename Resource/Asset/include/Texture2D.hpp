#pragma once
#include "Texture.hpp"
#include "UploadableTexture.hpp"
#include "UploadableTextureResource.hpp"
#include <cstdint>
#include <memory>
#include <vector>

using namespace MEngine::Core;
namespace MEngine::Resource
{

class Texture2D : public UploadableTexture
{
  protected:
    Texture2D() : UploadableTexture()
    {
    }

  public:
    /**
     * @brief Construct a new Texture 2 D object
     *
     * @param name
     * @param textureSetting
     * @param dynamic 是否创建后需要频繁更新数据
     */
    Texture2D(const std::string &name, const TextureSetting &textureSetting, bool dynamic = false)
        : UploadableTexture(name, textureSetting, dynamic)
    {
        mTextureSettings.setImageType(vk::ImageType::e2D).setArrayLayers(1).setFlags({});
        mResource = std::make_unique<UploadableTextureResource>(this);
    }
};
} // namespace MEngine::Resource
