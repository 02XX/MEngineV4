#pragma once
#include "RHIResourceType.hpp"
namespace MEngine::Platform
{
class RHIResource
{
  private:
    RHIResourceType mType{RHIResourceType::None};

  public:
    virtual ~RHIResource() = default;
    inline RHIResourceType GetType() const
    {
        return mType;
    }
};
} // namespace MEngine::Platform
