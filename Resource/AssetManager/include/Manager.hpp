#pragma once
#include "AssetMetadata.hpp"
#include "AssetURL.hpp"
#include "IManager.hpp"
#include "Logger.hpp"
#include "PendingResourceManager.hpp"
#include "RflEntity.hpp"
#include "Serialization.hpp"
#include "UUID.hpp"
#include <concepts>
#include <fstream>
#include <memory>
#include <ranges>
#include <rfl.hpp>
#include <rfl/flexbuf.hpp>
#include <unordered_map>

namespace MEngine::Resource
{
/**
 * @brief 抽象类
 *
 */
class Manager : public virtual IManager, public virtual Serialization
{
  protected:
    std::shared_ptr<Context> mContext{};

  protected:
    std::unordered_map<Core::UUID, AssetMetadata> mAssetMetadatas{};
    std::unordered_map<Core::UUID, std::weak_ptr<Asset>> mCachedAssets{};
    // Helper maps
    std::unordered_map<std::string, Core::UUID> mNameToIDMap{};

  public:
    Manager(std::shared_ptr<Context> context) : mContext(context)
    {
    }
    void Add(std::shared_ptr<Asset> asset) override
    {
        if (!asset)
        {
            LogError("Trying to add a null asset");
            return;
        }
        if (mCachedAssets.contains(asset->mID))
        {
            LogWarn("Asset with ID {} already exists. Overwriting.", asset->mID.ToString());
        }
        mCachedAssets[asset->mID] = asset;
        mNameToIDMap[asset->mName] = asset->mID;
    }
    std::shared_ptr<Asset> Get(const Core::UUID &id) override
    {
        if (mCachedAssets.contains(id) && !mCachedAssets.at(id).expired())
        {
            return mCachedAssets.at(id).lock();
        }
        return Load(mAssetMetadatas.at(id).mAssetURL);
    }
    std::shared_ptr<Asset> GetByName(const std::string &name) override
    {
        return Get(mNameToIDMap.at(name));
    }
    void Remove(const Core::UUID &id) override
    {
        if (mCachedAssets.contains(id))
        {
            mCachedAssets.erase(id);
        }
    }
};
} // namespace MEngine::Resource