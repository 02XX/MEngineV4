
#include "MPMCQueue.hpp"
#include "RHIResourceType.hpp"
#include <atomic>
#include <cstdint>

namespace MEngine::Platform
{
class RHIResource
{
  private:
    bool mIsLoaded{false};
    std::atomic<uint32_t> mRefCount{0};
    std::atomic<bool> mIsMarkedForDelete{false};
    static MPMCQueue<RHIResource *> PendingDeletes;

  protected:
    virtual ~RHIResource();
    void MarkForDelete();

  public:
    RHIResource();
    uint32_t AddRef();
    uint32_t Release();
    uint32_t GetRefCount() const;
    inline bool IsLoaded() const
    {
        return mIsLoaded;
    }
    //   public:
    //     // RHIResource(RHIResourceType InResourceType);
    //     RHIResource();

    //   protected:
    //     // RHI resources should only be destructed via the deletion queue,
    //     // so this is protected to prevent others from 'delete'ing these directly.
    //     virtual ~RHIResource();

    //   private:
    //     // Separate function to avoid force inlining this everywhere. Helps both for code size and performance.
    //     void MarkForDelete() const;

    //     // friend class FDynamicRHI;
    //     // friend class RHICommandListExecutor;
    //     // static void DeleteResources(TArray<RHIResource *> const &Resources);
    //     // static void GatherResourcesToDelete(TArray<RHIResource *> &OutResources, bool
    //     bIncludeExtendedLifetimeResources);

    //   public:
    //     uint32_t AddRef() const
    //     {
    //         // int32 NewValue = AtomicFlags.AddRef(std::memory_order_acquire);
    //         // checkSlow(NewValue > 0);
    //         // return uint32_t(NewValue);
    //     }

    //     uint32_t Release() const
    //     {
    //         // int32 NewValue = AtomicFlags.Release(std::memory_order_release);
    //         // check(NewValue >= 0);

    //         // if (NewValue == 0)
    //         // {
    //         //     MarkForDelete();
    //         // }
    //         // checkSlow(NewValue >= 0);
    //         // return uint32_t(NewValue);
    //     }

    //     uint32_t GetRefCount() const
    //     {
    //         int32 CurrentValue = AtomicFlags.GetNumRefs(std::memory_order_relaxed);
    //         checkSlow(CurrentValue >= 0);
    //         return uint32_t(CurrentValue);
    //     }
    //     inline RHIResourceType GetType() const
    //     {
    //         return ResourceType;
    //     }

    //     // FName GetOwnerName() const
    //     // {
    //     // }

    //     // void SetOwnerName(FName InOwnerName)
    //     // {
    //     // }

    //   private:
    //     const RHIResourceType ResourceType;
    //     bool mIsLoaded{false};
    //     // uint8 bAllowExtendLifetime : 1;
};
} // namespace MEngine::Platform
