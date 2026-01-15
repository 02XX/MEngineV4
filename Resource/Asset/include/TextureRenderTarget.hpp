#pragma once
#include "Texture.hpp"
namespace MEngine::Resource
{
class TextureRenderTarget : public Texture
{
    friend class TextureRenderTargetResource;

  protected:
  public:
    TextureRenderTarget(const std::string &name, const TextureSetting &importSetting,
                        const SamplerSetting &samplerSetting)
        : Texture(name, importSetting, samplerSetting)
    {
    }
    ~TextureRenderTarget() override = default;
};
} // namespace MEngine::Resource