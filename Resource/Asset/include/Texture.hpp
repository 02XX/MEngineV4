#pragma once
#include "Asset.hpp"
#include <string>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>

namespace MEngine::Resource
{

using TextureSetting = vk::ImageCreateInfo;
using SamplerSetting = vk::SamplerCreateInfo;
// https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/Engine/UTexture
class Texture : public Asset
{
  private:
  protected:
    TextureSetting mTextureSettings{};
    SamplerSetting mSamplerSettings{};

  protected:
    Texture() : Asset()
    {
    }

  public:
    Texture(const std::string &name, const TextureSetting &importSetting, const SamplerSetting &samplerSetting)
        : Asset(name), mTextureSettings(importSetting), mSamplerSettings(samplerSetting)
    {
    }
    ~Texture() override = default;
};
} // namespace MEngine::Resource
