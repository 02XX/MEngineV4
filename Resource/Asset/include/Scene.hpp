#pragma once
#include "Asset.hpp"
#include "ECS.hpp"
#include "SceneResource.hpp"
#include <memory>
namespace MEngine::Resource
{
class Scene : public Asset
{
  public:
    std::shared_ptr<Register> mRegistry;

  public:
    Scene(const std::string &name);
    ~Scene() override = default;
};
} // namespace MEngine::Resource