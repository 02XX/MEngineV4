#pragma once
#include "RHIPipeline.hpp"
namespace MEngine::Platform
{
class RHIComputePipeline final : public RHIPipeline
{
  protected:
    RHIComputePipeline() : RHIPipeline()
    {
    }

  public:
    ~RHIComputePipeline() override = default;
};
} // namespace MEngine::Platform