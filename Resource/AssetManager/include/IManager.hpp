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
class IManager : public virtual IPendingResourceManager
{
  public:
    virtual ~IManager() = default;
    virtual std::shared_ptr<Asset> Load(const AssetURL &url) = 0;
    virtual void Save(std::shared_ptr<Asset> asset, const AssetURL &url) = 0;
    virtual void Add(std::shared_ptr<Asset> asset) = 0;
    virtual std::shared_ptr<Asset> Get(const Core::UUID &id) const = 0;
    virtual std::shared_ptr<Asset> GetByName(const std::string &name) const = 0;
    virtual std::vector<std::shared_ptr<Asset>> GetAll() const = 0;
    virtual void Remove(const Core::UUID &id) = 0;

    virtual void DestroyAll() = 0;
};
} // namespace MEngine::Resource