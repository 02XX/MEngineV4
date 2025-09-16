#pragma once
#include "RHIResource.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
struct RHIPipelineLayoutDesc
{
    std::vector<vk::DescriptorSetLayout> mDescriptorSetLayouts{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};
    vk::PipelineLayoutCreateFlags mFlags{vk::PipelineLayoutCreateFlags()};
};
class RHIPipeline : public RHIResource
{
  protected:
    vk::UniquePipeline mPipeline{nullptr};
    vk::UniquePipelineLayout mPipelineLayout{nullptr};
    RHIPipelineLayoutDesc mPipelineLayoutDesc{};

  protected:
    RHIPipeline(const RHIPipelineLayoutDesc &layoutDesc);

  public:
    ~RHIPipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
};
} // namespace MEngine::Platform