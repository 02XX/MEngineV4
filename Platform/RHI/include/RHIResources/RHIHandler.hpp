#pragma once
#include "RHIResource.hpp"
#include <concepts>

namespace MEngine::Platform
{
template <std::derived_from<RHIResource> TResource> class RHIHandler
{
  private:
    TResource *mResource{nullptr};

  public:
    RHIHandler() : mResource(nullptr)
    {
    }
    explicit RHIHandler(TResource *resource) : mResource(resource)
    {
    }
    ~RHIHandler()
    {
        SafeRelease();
    }
    RHIHandler(const RHIHandler &other) : mResource(other.mResource)
    {
        if (mResource)
        {
            mResource->AddRef();
        }
    }
    RHIHandler(RHIHandler &&other) noexcept : mResource(other.mResource)
    {
        other.mResource = nullptr;
    }
    void operator=(const RHIHandler &other)
    {
        if (this != &other)
        {
            mResource = other.mResource;
            if (mResource)
            {
                mResource->AddRef();
            }
        }
    }
    void operator=(RHIHandler &&other) noexcept
    {
        if (this != &other)
        {
            mResource = other.mResource;
            other.mResource = nullptr;
        }
    }
    TResource *operator->() const
    {
        return mResource;
    }
    operator bool() const
    {
        return mResource != nullptr;
    }
    bool operator==(std::nullptr_t) const
    {
        return mResource == nullptr;
    }

    void SafeRelease()
    {
        if (mResource)
        {
            mResource->Release();
            mResource = nullptr;
        }
    }
    TResource *Get()
    {
        return mResource;
    }
};

} // namespace MEngine::Platform