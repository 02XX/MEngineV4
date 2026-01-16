#pragma once
#include "ConcurrentQueue.hpp"
#include "Context.hpp"
#include "ECS.hpp"
#include "OffscreenFrameResource.hpp"
#include "System.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>
inline constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
using namespace MEngine::Resource;
namespace MEngine::Function
{
inline ConcurrentQueue<vk::SubmitInfo2> PendingSubmissions{};
class RenderSystem : public System
{
  private:
    std::shared_ptr<Context> mContext{};
    // 使用bindless descriptor ！！！！！！！！！！！！！
  private:
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues{};
    OffscreenFrameResource *mOffscreenFrameResource{};

  public:
    RenderSystem(std::shared_ptr<Context> context, std::shared_ptr<Scene> scene,
                 std::shared_ptr<AssetManager> assetManager);
    ~RenderSystem() override;
    inline void SetOffscreenFrameResource(OffscreenFrameResource *frameResource)
    {
        mOffscreenFrameResource = frameResource;
    }
    void Init() override;
    void Update(double deltaTime) override;
    void Shutdown() override;

  private:
    void PrepareRenderQueues();
    void Prepare();
    void RenderGBuffer();
    void RenderLighting();
    void End();
};
} // namespace MEngine::Function