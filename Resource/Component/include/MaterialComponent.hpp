#pragma once
#include "Component.hpp"
#include "Material.hpp"

namespace MEngine::Resource
{
class MaterialComponent : public Component
{
  public:
    std::shared_ptr<Resource::Material> Material{nullptr};
};

} // namespace MEngine::Resource
