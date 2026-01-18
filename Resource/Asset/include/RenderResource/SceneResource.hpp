#pragma once
#include "ECS.hpp"
#include "Math.hpp"
#include "RenderResource.hpp"
#include <array>
#include <memory>
#include <vector>
using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Scene;

class SceneResource final : public RenderResource
{
  public:
  protected:
    Scene *mScene{nullptr};

  public:
    SceneResource(Scene *scene);
    ~SceneResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource
