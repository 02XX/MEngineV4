
#include "RHIResource.hpp"
#include "MPMCQueue.hpp"
#include "RHIHandler.hpp"
#include <atomic>
namespace MEngine::Platform
{
MPMCQueue<RHIResource *> PendingDeletes{};

void RHIResource::Release()
{

    mReferenceCount.fetch_sub(1, std::memory_order_release);
    if (mReferenceCount.load(std::memory_order_acquire) == 0)
    {
        MarkForDelete();
    }
}
void RHIResource::AddRef()
{
    mReferenceCount.fetch_add(1, std::memory_order_acquire);
}
void RHIResource::MarkForDelete()
{
    PendingDeletes.Produce(static_cast<RHIResource *>(this));
    mState = RHIResourceState::Deleted;
}
} // namespace MEngine::Platform
