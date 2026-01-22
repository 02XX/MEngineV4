#pragma once

#include "RenderResource.hpp"
#include <vector>
namespace MEngine::Resource
{
class Pipeline;
class PipelineResource : public RenderResource
{
  public:
    vk::Pipeline mPipeline{};
    vk::PipelineLayout mPipelineLayout{};

  public:
    PipelineResource(Pipeline *pipeline);

    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource