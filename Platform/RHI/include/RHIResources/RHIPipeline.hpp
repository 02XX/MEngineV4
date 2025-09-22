#pragma once
#include "RHIResource.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
struct RHIPipelineLayoutDesc
{
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> DescriptorSetLayoutBindings{};
    std::vector<vk::PushConstantRange> PushConstantRanges{};
    vk::PipelineLayoutCreateFlags Flags{vk::PipelineLayoutCreateFlags()};
};
class RHIPipeline : public RHIResource
{
  protected:
    vk::UniquePipeline mPipeline{nullptr};
    vk::UniquePipelineLayout mPipelineLayout{nullptr};
    RHIPipelineLayoutDesc mPipelineLayoutDesc{};
    std::vector<vk::UniqueDescriptorSetLayout> mDescriptorSetLayouts{};

  protected:
    RHIPipeline(const RHIPipelineLayoutDesc &layoutDesc);

  public:
    ~RHIPipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
    inline std::vector<vk::DescriptorSetLayout> GetDescriptorSetLayouts() const
    {
        std::vector<vk::DescriptorSetLayout> layouts;
        layouts.reserve(mDescriptorSetLayouts.size());
        for (const auto &layout : mDescriptorSetLayouts)
        {
            layouts.push_back(layout.get());
        }
        return layouts;
    }
};
} // namespace MEngine::Platform