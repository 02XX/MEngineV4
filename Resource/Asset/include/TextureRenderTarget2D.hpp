#pragma once
#include "TextureRenderTarget.hpp"
namespace MEngine::Resource
{
class TextureRenderTarget2D : public TextureRenderTarget
{
  protected:
    TextureRenderTarget2D() : TextureRenderTarget()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 1;
    }

  public:
    ~TextureRenderTarget2D() override = default;
};
} // namespace MEngine::Resource