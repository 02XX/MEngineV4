
#include "RHIResource.hpp"
#include <sys/stat.h>
namespace MEngine::Platform
{
MPMCQueue<RHIResource *> RHIResource::PendingDeletes{};
RHIResource::~RHIResource()
{
}
void RHIResource::MarkForDelete()
{
    if (!mIsMarkedForDelete.exchange(true))
    {
        PendingDeletes.Produce(const_cast<RHIResource *>(this));
    }
}

RHIResource::RHIResource()
{
}
uint32_t RHIResource::AddRef()
{
    mRefCount.fetch_add(1, std::memory_order_acquire);
    return mRefCount.load(std::memory_order_relaxed);
}
uint32_t RHIResource::Release()
{
    uint32_t RefCount = mRefCount.fetch_sub(1, std::memory_order_release);
    if (RefCount == 0)
    {
        MarkForDelete();
    }
    return RefCount;
}
uint32_t RHIResource::GetRefCount() const
{
    return mRefCount.load(std::memory_order_relaxed);
}

} // namespace MEngine::Platform
