#pragma once
#include "MPMCQueue.hpp"
#include "RHIResource.hpp"
#include <atomic>
#include <cstdint>

namespace MEngine::Platform
{
template <typename TResource> class RHIHandler;
extern MPMCQueue<RHIResource *> PendingDeletes;
template <typename TResource> class RHIHandler
{
  private:
    struct ControlBlock
    {
        std::atomic<uint32_t> refCount{0};
    };
    ControlBlock *mControlBlock{nullptr};
    void MarkForDeletion()
    {
        PendingDeletes.Produce(mResource);
    }
    TResource *mResource{nullptr};

  public:
    RHIHandler() : mResource(nullptr), mControlBlock(new ControlBlock())
    {
    }
    explicit RHIHandler(TResource *resource) : mResource(resource), mControlBlock(new ControlBlock())
    {
        mControlBlock->refCount = 1;
    }
    ~RHIHandler()
    {
        Release();
    }
    RHIHandler(const RHIHandler &other) : mControlBlock(other.mControlBlock), mResource(other.mResource)
    {
        mControlBlock->refCount++;
    }
    RHIHandler(RHIHandler &&other) noexcept : mControlBlock(other.mControlBlock), mResource(other.mResource)
    {
        other.mControlBlock = nullptr;
        other.mResource = nullptr;
    }
    void operator=(const RHIHandler &other)
    {
        if (this != &other)
        {
            if (mControlBlock && --mControlBlock->refCount == 0)
            {
                delete mControlBlock;
                MarkForDeletion();
            }
            mControlBlock = other.mControlBlock;
            mResource = other.mResource;
            mControlBlock->refCount++;
        }
    }
    void operator=(RHIHandler &&other) noexcept
    {
        if (this != &other)
        {
            if (mControlBlock && --mControlBlock->refCount == 0)
            {
                delete mControlBlock;
                MarkForDeletion();
            }
            mControlBlock = other.mControlBlock;
            mResource = other.mResource;
            mControlBlock->refCount++;
        }
    }

    uint32_t GetRefCount() const
    {
        return mControlBlock ? mControlBlock->refCount.load() : 0;
    }
    void Release()
    {
        if (mControlBlock && --mControlBlock->refCount == 0)
        {
            delete mControlBlock;
            MarkForDeletion();
        }
        mControlBlock = nullptr;
        mResource = nullptr;
    }
};

} // namespace MEngine::Platform