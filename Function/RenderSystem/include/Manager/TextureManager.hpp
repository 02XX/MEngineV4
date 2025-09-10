#pragma once
#include "ITextureManager.hpp"
#include "Manager.hpp"
#include <vulkan/vulkan.hpp>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class TextureManager final : public Manager<Texture>, public virtual ITextureManager
{
  public:
    ~TextureManager() override = default;
};
} // namespace MEngine::Function