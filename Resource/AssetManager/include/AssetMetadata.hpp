#pragma once
#include "AssetURL.hpp"
#include "UUID.hpp"
#include <vector>
namespace MEngine::Resource
{
class AssetMetadata final
{
  public:
    Core::UUID mUUID{};
    AssetURL mAssetURL{};
    std::vector<Core::UUID> mDependencies{};
    std::vector<std::string> mTags{};
    uint64_t mSizeInBytes{0};

  public:
    ~AssetMetadata() = default;
};
} // namespace MEngine::Resource