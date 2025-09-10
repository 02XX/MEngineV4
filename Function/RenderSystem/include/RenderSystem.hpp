#pragma once
#include "ECS.hpp"
#include "FrameBuffer.hpp"
#include "Scene.hpp"
#include "System.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace MEngine::Resource;
namespace MEngine::Function
{
class RenderSystem final : public System
{
  private:
    std::shared_ptr<VulkanContext> mVulkanContext{nullptr};
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues;
    std::vector<std::shared_ptr<FrameBuffer>> mFrameBuffers;
    uint32_t mCurrentFrameIndex{0};

  public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<VulkanContext> vulkanContext)
        : System(scene), mVulkanContext(vulkanContext)
    {
    }
    ~RenderSystem() override = default;
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;
    inline void SetFrameBuffers(std::vector<std::shared_ptr<FrameBuffer>> frameBuffers)
    {
        mFrameBuffers = frameBuffers;
    }

  private:
    void PrePareRenderQueues();
};
} // namespace MEngine::Function