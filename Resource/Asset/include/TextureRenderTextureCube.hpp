#pragma once
#include "TextureRenderTarget.hpp"
namespace MEngine::Resource
{
class TextureRenderTextureCube : public TextureRenderTarget
{
  private:
  protected:
    TextureRenderTextureCube() : TextureRenderTarget()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 6;
    }

  public:
    ~TextureRenderTextureCube() override = default;
};
} // namespace MEngine::Resource