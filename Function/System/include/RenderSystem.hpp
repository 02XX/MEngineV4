#pragma once
#include "ECS.hpp"
#include "FrameBuffer.hpp"
#include "LightComponent.hpp"
#include "Math.hpp"
#include "Pipeline.hpp"
#include "RHIBuffer.hpp"
#include "RHIContext.hpp"
#include "RHIDescriptorSet.hpp"
#include "RHIHandler.hpp"
#include "System.hpp"
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace MEngine::Function
{
struct LightParameters
{
    // base
    LightType LightType = LightType::Directional;

    float Intensity = 1.0f;

    // point
    float Radius = 10.0f;

    // spot
    float InnerConeAngle = 0.0f; // in radians
    float OuterConeAngle = 0.0f; // in radians
    int enable = 0;              // 是否启用光源，1表示启用，0表示禁用
    alignas(16) glm::vec3 Color = glm::vec3(1.0f);

    alignas(16) glm::vec3 Position = glm::vec3(0.0f);

    alignas(16) glm::vec3 Direction = glm::vec3(0.0f, 0.0f, 1.0f);
};
struct CameraParameters
{
    alignas(16) glm::vec3 Position = glm::vec3(0.0f);
    alignas(16) glm::vec3 Direction = glm::vec3(0.0f, 0.0f, -1.0f);
    alignas(16) glm::mat4 ProjectionMatrix = glm::identity<glm::mat4>();
    alignas(16) glm::mat4 ViewMatrix = glm::identity<glm::mat4>();
};
class RenderSystem : public System
{
  private:
    std::vector<std::shared_ptr<FrameBuffer>> mFrameBuffers{};
    std::vector<vk::UniqueCommandBuffer> mCommandBuffers{};
    std::vector<vk::UniqueSemaphore> mImageAvailableSemaphores{};
    std::vector<vk::UniqueFence> mInFlightFences{};
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues{};
    uint32_t mCurrentFrameBufferIndex{0};
    Entity mCameraEntity{};
    // RHIHandler<RHIBuffer> mGlobalUniformBufferHandler{};

  public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<AssetManager> assetManager);
    ~RenderSystem() override;
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;
    void SetFrameBuffers(std::vector<std::shared_ptr<FrameBuffer>> frameBuffers);
    inline vk::Semaphore GetImageAvailableSemaphore()
    {
        return mImageAvailableSemaphores[mCurrentFrameBufferIndex].get();
    }

  private:
    void PrepareGlobalResources();
    void UpdateGlobalResources(vk::DescriptorSet globalDescriptorSet);
    void PrepareRenderQueues();
    void PrePare();
    void RenderGBuffer();
    void RenderLighting();
    void End();
};
} // namespace MEngine::Function