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

    }

  public:
    ~TextureCubeArray() override = default;
};
} // namespace MEngine::Resource