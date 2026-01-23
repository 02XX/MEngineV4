#pragma once
#include "Asset.hpp"
#include "Context.hpp"
#include "RenderResource.hpp"
#include <memory>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
struct RenderContext
{
    std::shared_ptr<Context> Context;
    vk::CommandBuffer CommandBuffer;
};
class IPendingResourceManager
{

  public:
    using Task = std::function<void(RenderContext renderContext)>;

    virtual ~IPendingResourceManager() = default;
    // Only Called On Render Thread
    virtual void ProcessPendingInitResources(RenderContext renderContext) = 0;
    virtual void ProcessPendingUpdateResources(RenderContext renderContext) = 0;
    virtual void ProcessPendingDeletionResources(RenderContext renderContext) = 0;
    virtual void ProcessPendingTasks(RenderContext renderContext) = 0;
    // Main Thread
    virtual void PendingInit(RenderResource *resource) = 0;
    virtual void PendingUpdate(RenderResource *resource) = 0;
    virtual void PendingDelete(std::unique_ptr<RenderResource> resource) = 0;
    virtual void PendingTask(Task task) = 0;
};
} // namespace MEngine::Resource