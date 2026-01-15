#pragma once

#include "RenderResource.hpp"
#include <vector>
namespace MEngine::Resource
{
class PipelineResource : public RenderResource
{
  protected:
    vk::Pipeline mPipeline{};
    vk::PipelineLayout mPipelineLayout{};
    std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts{};

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> mDescriptorSetLayoutBindings{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

  public:
    PipelineResource(std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings,
                     std::vector<vk::PushConstantRange> pushConstantRanges)
        : mDescriptorSetLayoutBindings(descriptorSetLayoutBindings), mPushConstantRanges(pushConstantRanges)
    {
    }
    ~PipelineResource() override = default;
    virtual void InitRHI(std::shared_ptr<Context> context) override;
    virtual void ReleaseRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource