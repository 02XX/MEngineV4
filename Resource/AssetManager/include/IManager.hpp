#pragma once
#include "Asset.hpp"
#include "Context.hpp"
#include <concepts>
#include <memory>
#include <vector>

namespace MEngine::Resource
{
template <std::derived_from<Asset> TAsset> class IManager
{
  public:
    virtual ~IManager() = default;
    virtual void Add(std::shared_ptr<TAsset> asset) = 0;
    virtual std::shared_ptr<TAsset> Get(const Core::UUID &id) const = 0;
    virtual std::shared_ptr<TAsset> GetByName(const std::string &name) const = 0;
    virtual std::vector<std::shared_ptr<TAsset>> GetAll() const = 0;
    // TODO: 加入render thread task
    //  virtual void Update(std::shared_ptr<TAsset> asset) = 0;
    virtual void Remove(const Core::UUID &id) = 0;
    virtual void CreateDefault() = 0;
};
} // namespace MEngine::Resource