#pragma once
#include "Texture2D.hpp"

using namespace MEngine::Core;
namespace MEngine::Resource
{
class ShadowMapTexture2D : public Texture2D
{
  private:
  protected:
    ShadowMapTexture2D() : Texture2D()
    {
    }

  public:
    ~ShadowMapTexture2D() override = default;
};
} // namespace MEngine::Resource