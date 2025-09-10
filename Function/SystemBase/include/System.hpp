#pragma once
#include "ISystem.hpp"
#include "Scene.hpp"
#include <memory>
using namespace MEngine::Resource;
namespace MEngine::Function
{
class System : public ISystem
{
  protected:
    std::shared_ptr<Scene> mScene;

  public:
    System(std::shared_ptr<Scene> scene) : mScene(scene)
    {
    }
    virtual ~System() = default;
    inline std::shared_ptr<Scene> GetScene() const
    {
        return mScene;
    }
    inline void SetScene(std::shared_ptr<Scene> scene)
    {
        mScene = scene;
    }
};

} // namespace MEngine::Function