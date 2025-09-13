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
    }

  public:
    ~Texture2DArray() override = default;
};
} // namespace MEngine::Resource