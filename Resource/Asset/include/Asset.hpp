#pragma once
#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include <nlohmann/json_fwd.hpp>
#include <string>
using namespace MEngine::Core;
namespace MEngine::Resource
{
class Asset
{
    friend struct nlohmann::adl_serializer<Asset>;
    friend class AssetManager;

  protected:
    UUID mID{};
    std::string mName{"Unnamed"};
    bool mLoaded{false};
    Asset() : mID(UUIDGenerator::Instance().Create()), mName("Unnamed")
    {
    }

  public:
    virtual ~Asset() = default;
    virtual inline const UUID &GetID() const
    {
        return mID;
    }
    virtual inline const std::string &GetName() const
    {
        return mName;
    }
    /**
     * @brief 是否加载了Vulkan资源
     *
     * @return true
     * @return false
     */
    virtual inline bool IsLoaded() const
    {
        return mLoaded;
    }
};
} // namespace MEngine::Resource