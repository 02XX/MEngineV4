#pragma once
#include "Texture.hpp"

using namespace MEngine::Core;
namespace MEngine::Function
{
class Texture2DBuilder;
}
namespace MEngine::Resource
{
class Texture2D : public Texture
{
    friend class Function::Texture2DBuilder;

  protected:
    Texture2D() : Texture()
    {
    }

  public:
    ~Texture2D() override = default;
};
} // namespace MEngine::Resource