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
    }

  public:
    ~TextureCube() override = default;
};
} // namespace MEngine::Resource