#pragma once
#include "Asset.hpp"
#include "TextureResource.hpp"
#include <string>

namespace MEngine::Resource
{

using TextureSetting = vk::ImageCreateInfo;
using SamplerSetting = vk::SamplerCreateInfo;
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Engine/UTexture
// Abstract Class
class Texture : public Asset
{
    friend class TextureResource;

  private:
  protected:
    TextureSetting mTextureSettings{};
    SamplerSetting mSamplerSettings{};

  public:
    Texture(const std::string &name, const TextureSetting &importSetting, const SamplerSetting &samplerSetting)
        : Asset(name), mTextureSettings(importSetting), mSamplerSettings(samplerSetting)
    {
    }
    ~Texture() override = 0;
};
inline Texture::~Texture()
{
}
} // namespace MEngine::Resource
