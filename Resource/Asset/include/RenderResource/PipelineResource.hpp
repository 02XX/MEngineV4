#pragma once

#include "RenderResource.hpp"
#include <vector>
namespace MEngine::Resource
{
class Pipeline;
class PipelineResource : public RenderResource
{
  protected:
    vk::Pipeline mPipeline{};
    vk::PipelineLayout mPipelineLayout{};
    std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts{};

  public:
    PipelineResource(Pipeline *pipeline);
    ~PipelineResource() override = default;
    virtual void InitRHI(std::shared_ptr<Context> context) override;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource