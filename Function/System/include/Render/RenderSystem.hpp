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
#include <taskflow/taskflow.hpp>
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
    std::shared_ptr<Context> mContext{};
    // 使用bindless descriptor ！！！！！！！！！！！！！
  private:
    std::unordered_map<std::string, std::vector<Entity>> mRenderQueues{};
    OffscreenFrameResource *mOffscreenFrameResource{};

    tf::Executor mTransferExecutor{4};
    tf::Taskflow mTransferTaskflow{};

    std::vector<std::function<void(OffscreenFrameResource *frameResource)>> mPreRecord{};
    std::vector<std::function<void(OffscreenFrameResource *frameResource)>> mPreProcessPasses{};
    std::vector<std::function<void(OffscreenFrameResource *frameResource)>> mRenderPasses{};
    std::vector<std::function<void(OffscreenFrameResource *frameResource)>> mPostProcessPasses{};
    std::vector<std::function<void(OffscreenFrameResource *frameResource)>> mPostSubmit{};



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
    inline void PushPreRecord(std::function<void(OffscreenFrameResource *frameResource)> preRecord)
    {
        mPreRecord.push_back(preRecord);
    }
    inline void PushRenderPass(std::function<void(OffscreenFrameResource *frameResource)> renderPass)
    {
        mRenderPasses.push_back(renderPass);
    }
    inline void PushPreProcessPass(std::function<void(OffscreenFrameResource *frameResource)> preProcessPass)
    {
        mPreProcessPasses.push_back(preProcessPass);
    }
    inline void PushPostProcessPass(std::function<void(OffscreenFrameResource *frameResource)> postProcessPass)
    {
        mPostProcessPasses.push_back(postProcessPass);
    }
    inline void PushPostSubmitPass(std::function<void(OffscreenFrameResource *frameResource)> postSubmitPass)
    {
        mPostSubmit.push_back(postSubmitPass);
    }

  private:
    void Render();

    void UpdateMaterial();
    void PrepareRenderQueues();
    void GBuffer(OffscreenFrameResource *frameResource);
    void Lighting(OffscreenFrameResource *frameResource);
};
} // namespace MEngine::Function