#pragma once
#include "RHIPipeline.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
class RHIGraphicPipeline : public RHIPipeline
{
  protected:
    vk::UniquePipeline mPipeline{nullptr};
    vk::UniquePipelineLayout mPipelineLayout{nullptr};

  protected:
    RHIGraphicPipeline() : RHIPipeline()
    {
    }

  public:
    ~RHIGraphicPipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
    inline vk::PipelineLayout GetPipelineLayout() const
    {
        return mPipelineLayout.get();
    }
};
} // namespace MEngine::Platform