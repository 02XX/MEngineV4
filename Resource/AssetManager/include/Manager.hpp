#pragma once
#include "AssetMetadata.hpp"
#include "AssetURL.hpp"
#include "IManager.hpp"
#include "Logger.hpp"
#include "PendingResourceManager.hpp"
#include "RflEntity.hpp"
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
template <std::derived_from<Asset> TAsset, std::derived_from<RenderResource> TRenderResource, typename TAssetEntity>
class Manager : public virtual IManager
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

    std::shared_ptr<Asset> Load(const AssetURL &url) override
    {
        auto path = url.GetPath();
        // std::ifstream file(path, std::ios::binary);
        std::ifstream file(path);
        if (!file.is_open())
        {
            LogError("Failed to open file: {}", path.string());
            return nullptr;
        }
        auto result = rfl::json::read<TAssetEntity>(file);
        if (!result)
        {
            LogError("Failed to deserialize: {}, {}", path.string(), result.error().what());
            return nullptr;
        }
        Transformer<TAsset, TAssetEntity> transformer;
        auto asset = transformer.FromEntity(result.value());
        if (asset)
        {
            asset->mID = result.value().assetEntity.value_.id;
        }
        return asset;
    }
    void Save(std::shared_ptr<Asset> asset, const AssetURL &url) override
    {
        auto path = url.GetPath();
        std::ofstream file(path);
        if (!file.is_open())
        {
            LogError("Failed to open file for writing: {}", path.string());
            return;
        }
        Transformer<TAsset, TAssetEntity> transformer;
        auto entity = transformer.ToEntity(asset); 
        rfl::json::write(entity, file);
        file.close();
    }
};
} // namespace MEngine::Resource