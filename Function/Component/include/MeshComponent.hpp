#pragma once
#include "Component.hpp"
#include "StaticMesh.hpp"
#include <memory>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class MeshComponent : public Component
{
  public:
    ~MeshComponent() override = default;
    std::shared_ptr<StaticMesh> mesh;
};

} // namespace MEngine::Function