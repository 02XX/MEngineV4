#pragma once
#include "Component.hpp"
#include "Material.hpp"

namespace MEngine::Resource
{
class MaterialComponent : public Component
{
  public:
    ~MaterialComponent() override = default;
    std::shared_ptr<Resource::Material> Material{nullptr};
};

} // namespace MEngine::Function
