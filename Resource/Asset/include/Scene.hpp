#pragma once
#include "Asset.hpp"
#include "ECS.hpp"
#include <memory>

namespace MEngine::Resource
{
class Scene : public Asset
{
  public:
    std::shared_ptr<Register> mRegistry;

  public:
    Scene(const std::string &name) : Asset(name)
    {
        mRegistry = std::make_shared<Register>();
    }
    virtual ~Scene() = default;
};
} // namespace MEngine::Resource