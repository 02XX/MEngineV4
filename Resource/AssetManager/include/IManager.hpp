#pragma once

#include "Asset.hpp"
#include "AssetURL.hpp"
#include "Context.hpp"
#include "IPendingResourceManager.hpp"
#include <concepts>
#include <memory>
#include <vector>

namespace MEngine::Resource
{
class IManager : public virtual IPendingResourceManager, public std::enable_shared_from_this<IManager>
{
  public:
    virtual ~IManager() = default;

    virtual std::shared_ptr<Asset> Load(const AssetURL &url) = 0;
    virtual void Save(std::shared_ptr<Asset> asset, const AssetURL &url) = 0;

    virtual std::shared_ptr<Asset> Import(const AssetURL &url);
    virtual void Export(std::shared_ptr<Asset> asset, const AssetURL &url);

    virtual void Add(std::shared_ptr<Asset> asset) = 0;
    virtual std::shared_ptr<Asset> Get(const Core::UUID &id) = 0;
    virtual std::shared_ptr<Asset> GetByName(const std::string &name) = 0;
    virtual void Remove(const Core::UUID &id) = 0;
    template <std::derived_from<Asset> TAsset> std::shared_ptr<TAsset> GetAs(const Core::UUID &id)
    {
        return std::static_pointer_cast<TAsset>(Get(id));
    }
    template <std::derived_from<Asset> TAsset> std::shared_ptr<TAsset> GetByNameAs(const std::string &name)
    {
        return std::static_pointer_cast<TAsset>(GetByName(name));
    }
};
} // namespace MEngine::Resource