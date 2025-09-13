#pragma once
#include "MPMCQueue.hpp"
#include "RHIHandler.hpp"
#include "RHIResource.hpp"

namespace MEngine::Platform
{
class RHIResourceManager
{
  private:
    MPMCQueue<RHIHandler<RHIResource>> mPendingDeletes;
    RHIResourceManager() = default;

  public:
    ~RHIResourceManager() = default;
    RHIResourceManager(const RHIResourceManager &) = delete;
    RHIResourceManager(RHIResourceManager &&) = delete;
    void operator=(const RHIResourceManager &) = delete;
    void operator=(RHIResourceManager &&) = delete;
    static RHIResourceManager &Instance();
};
} // namespace MEngine::Platform