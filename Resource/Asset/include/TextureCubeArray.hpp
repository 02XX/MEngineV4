#pragma once
#include "Texture.hpp"
namespace MEngine::Resource
{
class TextureCubeArray : public Texture
{
  private:
  protected:
    TextureCubeArray() : Texture()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 6 * 6;
    }

  public:
    ~TextureCubeArray() override = default;
};
} // namespace MEngine::Resource