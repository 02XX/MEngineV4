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
    }

  public:
    virtual ~Scene() = default;
    inline std::shared_ptr<Register> GetRegistry() const
    {
        return mRegistry;
    }
};
} // namespace MEngine::Resource