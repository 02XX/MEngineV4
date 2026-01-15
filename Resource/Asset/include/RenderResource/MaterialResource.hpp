#pragma once
#include "RenderResource.hpp"
#include <array>
#include <cstdint>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Material;
// Abstract class
class MaterialResource : public RenderResource
{
  public:
    MaterialResource(Material *material);
    ~MaterialResource() override = default;
};
} // namespace MEngine::Resource