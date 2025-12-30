#pragma once
#include "ECS.hpp"
#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
#include "RHIDescriptorSet.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
#include <array>
#include <vector>

using namespace MEngine::Platform;
namespace MEngine::Resource
{

class Scene;
class SceneResource final : public RenderResource
{
  protected:
    Scene *mScene{nullptr};
    std::array<RHIHandler<RHIBuffer>, MAX_FRAMES_IN_FLIGHT> mSceneUniformBufferHandlers{};
    std::array<RHIHandler<RHIDescriptorSet>, MAX_FRAMES_IN_FLIGHT> mSceneDescriptorSets{};
    Entity mCameraEntity = NullEntity;

  public:
    SceneResource(Scene *scene) : RenderResource(), mScene(scene)
    {
    }
    ~SceneResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    void UpdateSceneUBO(int frameIndex);
};
} // namespace MEngine::Resource
