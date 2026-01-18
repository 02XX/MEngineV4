#pragma once
#include "ConcurrentQueue.hpp"
#include "IManager.hpp"
#include "Logger.hpp"
#include "UUID.hpp"
#include <concepts>
#include <memory>
#include <ranges>
#include <unordered_map>

namespace MEngine::Resource
{
template <std::derived_from<Asset> TAsset> class Manager : public virtual IManager<TAsset>
{
  protected:
    std::unordered_map<Core::UUID, std::shared_ptr<TAsset>> mAssets;
    std::unordered_map<std::string, Core::UUID> mNameToIDMap;

  public:
    ~Manager() override = default;
    virtual void Add(std::shared_ptr<TAsset> asset) override
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
        mAssets[asset->GetID()] = asset;
        mNameToIDMap[asset->GetName()] = asset->GetID();
    }
    virtual std::shared_ptr<TAsset> Get(const Core::UUID &id) const override
    {
        if (mAssets.contains(id))
        {
            return mAssets.at(id);
        }
        LogWarn("Asset with ID {} not found", id.ToString());
        return nullptr;
    }
    virtual std::shared_ptr<TAsset> GetByName(const std::string &name) const override
    {
        if (mNameToIDMap.contains(name))
        {
            return Get(mNameToIDMap.at(name));
        }
        LogWarn("Asset with name {} not found", name);
        return nullptr;
    }
    virtual std::vector<std::shared_ptr<TAsset>> GetAll() const override
    {
        return mAssets | std::views::values | std::ranges::to<std::vector<std::shared_ptr<TAsset>>>();
    }
    virtual void Remove(const Core::UUID &id) override
    {
        if (mAssets.contains(id))
        {
            mAssets.erase(id);
        }
    }
};
} // namespace MEngine::Resource