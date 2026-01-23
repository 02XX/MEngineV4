#pragma once
#include "Component.hpp"
#include "Mesh.hpp"
#include <memory>
namespace MEngine::Resource
{
class MeshComponent : public Component
{
  public:
    std::shared_ptr<Resource::Mesh> Mesh{nullptr};
};

} // namespace MEngine::Resource