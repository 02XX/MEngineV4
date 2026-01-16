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
struct SceneParameter
{
    Matrix4 ViewMatrix;
    Matrix4 ProjectionMatrix;
    Vector3 CameraPosition;
};
class SceneResource final : public RenderResource
{
  public:
    vk::DeviceAddress mSceneSSBOAddress{};

  protected:
    vk::Buffer mSceneSSBO{nullptr};
    VmaAllocation mSceneSSBOAllocation{};
    VmaAllocationInfo mSceneSSBOAllocationInfo{};
    Scene *mScene{nullptr};

  public:
    SceneResource(Scene *scene);
    ~SceneResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    void UpdateSceneUBO(SceneParameter sceneParams);
};
} // namespace MEngine::Resource
