#pragma once
#include "Component.hpp"
#include "Material.hpp"

using namespace MEngine::Resource;
namespace MEngine::Function
{
class MaterialComponent : public Component
{
  public:
    ~MaterialComponent() override = default;
    std::shared_ptr<Material> material;
};

} // namespace MEngine::Function
