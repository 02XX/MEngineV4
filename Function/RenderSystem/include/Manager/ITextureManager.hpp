#pragma once
#include "IManager.hpp"
#include "Texture.hpp"
#include <vulkan/vulkan.hpp>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class ITextureManager : public virtual IManager<Texture>
{
  public:
    ~ITextureManager() override = default;
};
} // namespace MEngine::Function