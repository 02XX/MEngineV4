#pragma once
#include "RHIResourceType.hpp"
namespace MEngine::Platform
{
class RHIResource
{
  protected:
    RHIResourceType mType{RHIResourceType::None};
    bool mIsLoaded{false};
    bool mIsDirty{false};

  public:
    virtual ~RHIResource() = default;
    inline RHIResourceType GetType() const
    {
        return mType;
    }
    inline bool IsLoaded() const
    {
        return mIsLoaded;
    }
    inline bool IsDirty() const
    {
        return mIsDirty;
    }
};
} // namespace MEngine::Platform
