#pragma once
#include "IManager.hpp"
#include "Logger.hpp"
#include "PendingResourceManager.hpp"
#include "UUID.hpp"
#include <concepts>
#include <memory>
#include <ranges>
#include <unordered_map>

namespace MEngine::Resource
{
template <std::derived_from<Asset> TAsset, std::derived_from<RenderResource> TRenderResource>
class Manager : public virtual IManager, public PendingResourceManager<TRenderResource>
{
  protected:
    std::shared_ptr<Context> mContext{};

  protected:
    std::unordered_map<Core::UUID, std::shared_ptr<TAsset>> mAssets{};
    std::unordered_map<std::string, Core::UUID> mNameToIDMap;

  public:
    Manager(std::shared_ptr<Context> context) : mContext(context)
    {
    }
    std::shared_ptr<TAsset> GetAsset(const Core::UUID &id) const
    {
        return std::static_pointer_cast<TAsset>(Get(id));
    }
    std::shared_ptr<TAsset> GetAssetByName(const std::string &name) const
    {
        return std::static_pointer_cast<TAsset>(GetByName(name));
    }

    void Add(std::shared_ptr<Asset> asset) override
    {
        if (!asset)
        {
            LogError("Trying to add a null asset");
            return;
        }
        if (mAssets.contains(asset->GetID()))
        {
            LogWarn("Asset with ID {} already exists. Overwriting.", asset->GetID().ToString());
        }
        mAssets[asset->GetID()] = std::static_pointer_cast<TAsset>(asset);
        mNameToIDMap[asset->GetName()] = asset->GetID();
    }
    std::shared_ptr<Asset> Get(const Core::UUID &id) const override
    {
        if (mAssets.contains(id))
        {
            return mAssets.at(id);
        }
        return nullptr;
    }
    std::shared_ptr<Asset> GetByName(const std::string &name) const override
    {
        if (mNameToIDMap.contains(name))
        {
            return Get(mNameToIDMap.at(name));
        }
        return nullptr;
    }
    std::vector<std::shared_ptr<Asset>> GetAll() const override
    {
        return mAssets | std::views::values | std::ranges::to<std::vector<std::shared_ptr<Asset>>>();
    }
    void Remove(const Core::UUID &id) override
    {
        if (mAssets.contains(id))
        {
            mAssets.erase(id);
        }
    }
    void ProcessPendingDeletionResources(RenderContext renderContext) override
    {
        // for (auto &[id, asset] : mAssets)
        // {
        //     if (asset.use_count() == 1)
        //     {
        //         PendingDelete(std::move(asset->mResource));
        //     }
        // }
        PendingResourceManager<TRenderResource>::ProcessPendingDeletionResources(renderContext);
    }
    void DestroyAll() override
    {
        mAssets.clear();
        mNameToIDMap.clear();
    }
};
} // namespace MEngine::Resource