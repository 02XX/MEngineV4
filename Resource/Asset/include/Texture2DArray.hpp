#pragma once
#include "Texture.hpp"
namespace MEngine::Resource
{
class Texture2DArray : public Texture
{
  private:
  protected:
    Texture2DArray() : Texture()
    {
        mType = vk::ImageType::e2D;
        mArrayLevel = 6;
    }

  public:
    ~Texture2DArray() override = default;
};
} // namespace MEngine::Resource