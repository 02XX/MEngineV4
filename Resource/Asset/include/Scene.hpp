#pragma once
#include "Asset.hpp"
#include "ECS.hpp"
#include <memory>

namespace MEngine::Resource
{
class Scene : public Asset
{
  private:
    std::shared_ptr<Register> mRegistry;

  protected:
    Scene() : Asset()
    {
        mName = "Scene";
        mRegistry = std::make_shared<Register>();
    }

  public:
    Scene(const std::string &name) : Asset(name)
    {
        mRegistry = std::make_shared<Register>();
    }
    virtual ~Scene() = default;
    inline std::shared_ptr<Register> GetRegistry() const
    {
        return mRegistry;
    }
};
} // namespace MEngine::Resource