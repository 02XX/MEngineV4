#pragma once
#include "Texture.hpp"
namespace MEngine::Resource
{
class TextureRenderTarget : public Texture
{
  protected:
  protected:
    TextureRenderTarget() : Texture()
    {
    }

  public:
    TextureRenderTarget(const std::string &name, const TextureSetting &importSetting,
                        const SamplerSetting &samplerSetting)
        : Texture(name, importSetting, samplerSetting)
    {
    }
    ~TextureRenderTarget() override = default;
};
} // namespace MEngine::Resource