#pragma once
#include "ConcurrentQueue.hpp"
#include "Context.hpp"
#include "ECS.hpp"
#include "FrameResource.hpp"
#include "System.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
inline constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
using namespace MEngine::Resource;
namespace MEngine::Function
{
inline ConcurrentQueue<vk::SubmitInfo2> PendingSubmissions{};
class RenderSystem : public System
{
  private:
    std::shared_ptr<Context> mContext;

  private:
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues{};
    std::array<std::unique_ptr<FrameResource>, MAX_FRAMES_IN_FLIGHT> mFrameResources{};
    uint32_t mCurrentFrameBufferIndex{0};

  public:
    RenderSystem(std::shared_ptr<Context> context);
    ~RenderSystem() override;
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;
    inline FrameResource *GetFrameResource(uint32_t index) const
    {
        return mFrameResources[index].get();
    }
    void Submit();

  private:
    // void PrepareGlobalResources();
    // void UpdateGlobalResources(vk::DescriptorSet globalDescriptorSet);
    // void PrepareRenderQueues();
    void Prepare();
    void RenderGBuffer();
    void RenderLighting();
    void End();
};
} // namespace MEngine::Function