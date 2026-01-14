#pragma once
#include "TextureRenderTarget.hpp"
#include "TextureRenderTarget2DResource.hpp"

namespace MEngine::Resource
{
using namespace MEngine::Platform;
class TextureRenderTarget2D : public TextureRenderTarget
{
    friend class TextureRenderTarget2DResource;

  private:
  public:
    TextureRenderTarget2D(const std::string &name, const TextureSetting &importSetting,
                          const SamplerSetting &samplerSetting)
        : TextureRenderTarget(name, importSetting, samplerSetting)
    {
        mResource = std::make_unique<TextureRenderTarget2DResource>(importSetting, samplerSetting);
    }
    ~TextureRenderTarget2D() override = default;
};
} // namespace MEngine::Resource