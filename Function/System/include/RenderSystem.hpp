#pragma once
#include "ECS.hpp"
#include "FrameResource.hpp"
#include "RHIContext.hpp"
#include "System.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace MEngine::Function
{

class RenderSystem : public System
{
    using RenderCallback = std::function<void(FrameResource &)>;

  private:
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues{};
    std::array<FrameResource, MAX_FRAMES_IN_FLIGHT> mFrameResources{};
    uint32_t mCurrentFrameBufferIndex{0};
    RenderCallback mRenderCallbacks{};

  public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<AssetManager> assetManager);
    ~RenderSystem() override;
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;
    inline FrameResource &GetCurrentFrameResource()
    {
        return mFrameResources[mCurrentFrameBufferIndex];
    }
    void SetRenderCallback(const RenderCallback &callback)
    {
        mRenderCallbacks = callback;
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