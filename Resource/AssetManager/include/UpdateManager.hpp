#pragma once

#include "IManager.hpp"
#include "Manager.hpp"
#include <cstddef>
namespace MEngine::Resource
{
template <std::derived_from<Asset> TAsset> class UpdateManager : public virtual IManager<TAsset>, public Manager<TAsset>
{
  protected:
    ConcurrentQueue<std::shared_ptr<TAsset>> mPendingUpdateAssets{};
    
  public:
    ~UpdateManager() override = default;
    inline void PushPendingUpdateAsset(std::shared_ptr<TAsset> asset)
    {
        mPendingUpdateAssets.Push(asset);
    }
    inline size_t GetPendingUpdateAssetCount() const
    {
        return mPendingUpdateAssets.Size();
    }
    virtual void UpdateAssetRenderResource(std::shared_ptr<Context> context, vk::CommandBuffer commandBuffer,
                                           vk::CommandBufferInheritanceInfo *inheritanceInfo = nullptr) = 0;
};
} // namespace MEngine::Resource