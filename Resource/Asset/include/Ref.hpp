#pragma once
#include "Asset.hpp"
#include <concepts>
namespace MEngine::Resource
{
template <std::derived_from<Asset> TAsset> class Ref
{
  private:
    TAsset *mAssetPtr{nullptr};

  public:
    Ref() noexcept = default;
    explicit Ref(TAsset *assetPtr) noexcept : mAssetPtr(assetPtr)
    {
        if (mAssetPtr)
        {
            mAssetPtr->AddRef();
        }
    }
    Ref(const Ref &other) noexcept : mAssetPtr(other.mAssetPtr)
    {
        if (mAssetPtr)
        {
            mAssetPtr->AddRef();
        }
    }
    Ref &operator=(const Ref &other) noexcept
    {
        if (this != &other)
        {
            if (mAssetPtr)
            {
                mAssetPtr->UnRef();
            }
            mAssetPtr = other.mAssetPtr;
            if (mAssetPtr)
            {
                mAssetPtr->AddRef();
            }
        }
        return *this;
    }
    Ref(Ref &&other) noexcept : mAssetPtr(other.mAssetPtr)
    {
        other.mAssetPtr = nullptr;
    }
    Ref &operator=(Ref &&other) noexcept
    {
        if (this != &other)
        {
            if (mAssetPtr)
            {
                mAssetPtr->UnRef();
            }
            mAssetPtr = other.mAssetPtr;
            other.mAssetPtr = nullptr;
        }
        return *this;
    }
    ~Ref() noexcept
    {
        if (mAssetPtr)
        {
            mAssetPtr->UnRef();
        }
    }
    explicit operator bool() const noexcept
    {
        return mAssetPtr != nullptr;
    }
    TAsset *operator->() const noexcept
    {
        return mAssetPtr;
    }
    TAsset &operator*() const noexcept
    {
        return *mAssetPtr;
    }
    TAsset *Get() const noexcept
    {
        return mAssetPtr;
    }
};
} // namespace MEngine::Resource