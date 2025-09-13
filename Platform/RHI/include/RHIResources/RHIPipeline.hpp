#pragma once
#include "RHIResource.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
class RHIPipeline : public RHIResource
{
  protected:
    vk::UniquePipeline mPipeline{nullptr};
    vk::UniquePipelineLayout mPipelineLayout{nullptr};

  protected:
    RHIPipeline() : RHIResource()
    {
    }

  public:
    ~RHIPipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
};
} // namespace MEngine::Platform