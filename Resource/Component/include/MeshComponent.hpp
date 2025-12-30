#pragma once
#include "Component.hpp"
#include "StaticMesh.hpp"
#include <memory>
namespace MEngine::Resource
{
class MeshComponent : public Component
{
  public:
    ~MeshComponent() override = default;
    std::shared_ptr<StaticMesh> Mesh{nullptr};
};

} // namespace MEngine::Function