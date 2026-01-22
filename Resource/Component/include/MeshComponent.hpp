#pragma once
#include "Component.hpp"
#include "StaticMesh.hpp"
#include <memory>
namespace MEngine::Resource
{
class MeshComponent : public Component
{
  public:
    std::shared_ptr<Resource::StaticMesh> Mesh{nullptr};
};

} // namespace MEngine::Resource