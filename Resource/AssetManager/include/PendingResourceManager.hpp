#pragma once
#include "ConcurrentQueue.hpp"
#include "IPendingResourceManager.hpp"
#include "RenderResource.hpp"
#include <algorithm>
#include <concepts>
#include <memory>
#include <ranges>
#include <unordered_set>
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
        std::unordered_set<TRenderResource *> resources;
        TRenderResource *resource;
        while (queue.TryPop(resource))
        {
            resources.insert(resource);
        }
        return resources | std::ranges::to<std::vector<TRenderResource *>>();
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