#pragma once
#include "Logger.hpp"
#include "RenderResource.hpp"
#include "UUID.hpp"
#include "UUIDGenerator.hpp"
#include <gtest/gtest_prod.h>
#include <memory>
#include <nlohmann/json.hpp>
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
    std::unique_ptr<RenderResource> mResource{};
    Asset() : mID(UUID{}), mName("Unnamed")
    {
    }

  public:
    Asset(const std::string &name) : mID(UUIDGenerator::Instance().Create()), mName(name)
    {
    }
    virtual ~Asset()
    {
        LogDebug("Destroying Asset base {}", mName);
    };
    virtual inline const UUID &GetID() const
    {
        return mID;
    }
    virtual inline const std::string &GetName() const
    {
        return mName;
    }
    inline RenderResource *GetResource() const
    {
        if (!mResource.get())
        {
            LogWarn("Asset {} has no associated resource", mName);
        }
        return mResource.get();
    }
    template <typename T> inline T *GetResourceAs() const
    {
        if (auto res = dynamic_cast<T *>(GetResource()))
        {
            return res;
        }
        LogError("Failed to cast resource of asset {} to {}", mName, typeid(T).name());
        return nullptr;
    }
};
} // namespace MEngine::Resource