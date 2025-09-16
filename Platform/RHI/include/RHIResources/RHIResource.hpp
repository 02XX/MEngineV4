#pragma once
#include "MPMCQueue.hpp"
#include "RHIResourceType.hpp"
#include <atomic>

namespace MEngine::Platform
{

enum class RHIResourceState
{
    UnLoad,
    Loaded,
    Deleted,
    Dirty
};
class RHIResource
{

  protected:
    RHIResourceType mType{RHIResourceType::None};
    std::atomic_uint32_t mReferenceCount{1};
    RHIResourceState mState{RHIResourceState::UnLoad};

  public:
    virtual ~RHIResource() = default;
    inline RHIResourceType GetType() const
    {
        return mType;
    }
    inline RHIResourceState GetState() const
    {
        return mState;
    }
    void MarkForDelete();
    inline uint32_t GetRefCount() const
    {
        return mReferenceCount.load(std::memory_order_acquire);
    }
    void AddRef();
    void Release();
};
extern MPMCQueue<RHIResource *> PendingDeletes;
} // namespace MEngine::Platform
