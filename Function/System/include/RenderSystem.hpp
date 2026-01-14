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
inline constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
using namespace MEngine::Resource;
namespace MEngine::Function
{

class RenderSystem : public System
{
  private:
    std::shared_ptr<Context> mContext;

  private:
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues{};
    std::array<std::unique_ptr<FrameResource>, MAX_FRAMES_IN_FLIGHT> mFrameResources{};
    uint32_t mCurrentFrameBufferIndex{0};
    ConcurrentQueue<std::function<void(vk::Queue)>> mSubmitQueue{};

  public:
    RenderSystem(std::shared_ptr<Context> context);
    ~RenderSystem() override;
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;
    inline FrameResource *GetCurrentFrameResource() const
    {
        return mFrameResources[mCurrentFrameBufferIndex].get();
    }

  private:
    // void PrepareGlobalResources();
    // void UpdateGlobalResources(vk::DescriptorSet globalDescriptorSet);
    // void PrepareRenderQueues();
    void Prepare();
    void RenderGBuffer();
    void RenderLighting();
    void End();
    void Submit();
};
} // namespace MEngine::Function