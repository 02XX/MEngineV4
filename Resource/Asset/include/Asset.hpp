#pragma once
#include "UUID.hpp"

#include "Logger.hpp"
#include "RenderResource.hpp"
#include "UUIDGenerator.hpp"
#include <memory>
#include <string>

using namespace MEngine::Core;
namespace MEngine::Resource
{
class Asset
{
  protected:
    MEngine::Core::UUID mID{};
    std::string mName{"Unnamed"};
    std::unique_ptr<RenderResource> mResource{};
    Asset() : mID(MEngine::Core::UUID{}), mName("Unnamed")
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
    virtual inline const MEngine::Core::UUID &GetID() const
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