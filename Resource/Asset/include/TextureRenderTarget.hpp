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
        mFormat = vk::Format::eR32G32B32A32Sfloat;
    }

  public:
    ~TextureRenderTarget() override = default;
};
} // namespace MEngine::Resource