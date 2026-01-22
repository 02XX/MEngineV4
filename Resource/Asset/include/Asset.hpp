#pragma once
#include "UUID.hpp"

#include "Logger.hpp"
#include "RenderResource.hpp"
#include "UUIDGenerator.hpp"
#include <atomic>
#include <cstddef>
#include <memory>
#include <string>

using namespace MEngine::Core;
namespace MEngine::Resource
{
enum class AssetType
{
    Unknown,
    Shader,
    Texture,
    Mesh,
    Pipeline,
    Material,
};
class Asset
{
    friend class AssetManager;

  protected:
    std::atomic<size_t> mRefCount{0};
    MEngine::Core::UUID mID{};
    std::string mName{"Unnamed"};
    AssetType mAssetType{AssetType::Unknown};
    Asset() : mID(MEngine::Core::UUID{}), mName("Unnamed")
    {
    }

  public:
    std::unique_ptr<RenderResource> mResource{};
    Asset(const std::string &name) : mID(UUIDGenerator::Instance().Create()), mName(name)
    {
        // PushPendingInit(); // use lazy init instead
    }
    virtual ~Asset()
    {
        PendingDeletion();
    }
    virtual inline const MEngine::Core::UUID &GetID() const
    {
        return mID;
    }
    virtual inline const std::string &GetName() const
    {
        return mName;
    }
    virtual inline void SetName(const std::string &name)
    {
        mName = name;
    }
    virtual inline void SetID(const MEngine::Core::UUID &id)
    {
        mID = id;
    }
    AssetType GetAssetType() const
    {
        return mAssetType;
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
    void PendingInit();
    void PendingUpdate();
    void PendingDeletion();
    void AddRef()
    {
        mRefCount.fetch_add(1, std::memory_order_acq_rel);
    }
    void UnRef()
    {
        if (mRefCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
        {
            delete this;
        }
    }
    size_t GetRefCount() const
    {
        return mRefCount.load(std::memory_order_relaxed);
    }
};
} // namespace MEngine::Resource