#pragma once
#include "Asset.hpp"
#include "AssetURL.hpp"
#include <memory>
namespace MEngine::Resource
{
class ISerialization
{
  public:
    virtual ~ISerialization() = default;
    virtual std::shared_ptr<Asset> Load(const AssetURL &url) = 0;
    virtual void Save(std::shared_ptr<Asset> asset, const AssetURL &url) = 0;
};
} // namespace MEngine::Resource