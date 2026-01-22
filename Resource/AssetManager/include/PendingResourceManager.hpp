#pragma once
#include "ConcurrentQueue.hpp"
#include "IPendingResourceManager.hpp"
#include "RenderResource.hpp"
#include <concepts>
#include <memory>
#include <vector>

namespace MEngine::Resource
{

template <std::derived_from<RenderResource> TRenderResource>
class PendingResourceManager : public virtual IPendingResourceManager
{

  public:
    Core::ConcurrentQueue<TRenderResource *> mPendingInitResources{};
    Core::ConcurrentQueue<TRenderResource *> mPendingUpdateResources{};
    Core::ConcurrentQueue<std::unique_ptr<TRenderResource>> mPendingDeletion{};

  public:
    std::vector<TRenderResource *> ToVector(Core::ConcurrentQueue<TRenderResource *> &queue)
    {
        std::vector<TRenderResource *> resources;
        TRenderResource *resource;
        while (queue.TryPop(resource))
        {
            resources.push_back(resource);
        }
        return resources;
    }
    void ProcessPendingInitResources(RenderContext renderContext) override
    {
        auto resourcesToInit = ToVector(mPendingInitResources);
        for (auto resource : resourcesToInit)
        {
            resource->InitResource(renderContext.Context);
        }
    }
    void ProcessPendingUpdateResources(RenderContext renderContext) override
    {
    }
    void ProcessPendingDeletionResources(RenderContext renderContext) override
    {
        std::unique_ptr<TRenderResource> resource;
        while (mPendingDeletion.TryPop(resource))
        {
            resource->ReleaseResource(renderContext.Context);
        }
    }
    void PendingInit(RenderResource *resource) override
    {
        mPendingInitResources.Push(static_cast<TRenderResource *>(resource));
    }
    void PendingUpdate(RenderResource *resource) override
    {
        mPendingUpdateResources.Push(static_cast<TRenderResource *>(resource));
    }
    void PendingDelete(std::unique_ptr<RenderResource> resource) override
    {
        mPendingDeletion.Push(std::unique_ptr<TRenderResource>(static_cast<TRenderResource *>(resource.release())));
    }
};
} // namespace MEngine::Resource