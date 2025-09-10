#pragma once
#include "Texture.hpp"
namespace MEngine::Resource
{
class TextureCube : public Texture
{
  private:
  protected:
    TextureCube() : Texture()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 6;
    }

  public:
    ~TextureCube() override = default;
};
} // namespace MEngine::Resource