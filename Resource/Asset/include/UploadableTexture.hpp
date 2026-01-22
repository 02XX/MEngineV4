#pragma once
#include "Texture.hpp"
#include "UploadableTextureResource.hpp"
#include <cstdint>
#include <memory>
#include <vector>

using namespace MEngine::Core;
namespace MEngine::Resource
{
// Abstract class
class UploadableTexture : public Texture
{
    friend class UploadableTextureResource;

  public:
    std::vector<MipMap> mTextureDatas{}; //[mipLevel]
    bool mDynamic{false};

  protected:
    UploadableTexture() : Texture()
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
    UploadableTexture(const std::string &name, const TextureSetting &textureSetting, bool dynamic = false)
        : Texture(name, textureSetting), mDynamic(dynamic)
    {
    }
};
} // namespace MEngine::Resource
