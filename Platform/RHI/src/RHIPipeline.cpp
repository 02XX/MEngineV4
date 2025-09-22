#include "RHIPipeline.hpp"

#include "RHIContext.hpp"
#include <vector>

namespace MEngine::Platform
{
RHIPipeline::RHIPipeline(const RHIPipelineLayoutDesc &layoutDesc) : RHIResource(), mPipelineLayoutDesc(layoutDesc)
{
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    for (auto &bindings : mPipelineLayoutDesc.DescriptorSetLayoutBindings)
    {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
        descriptorSetLayoutCreateInfo.setBindings(bindings);
        auto descriptorSetLayout =
            RHIContext::Instance().GetDevice().createDescriptorSetLayoutUnique(descriptorSetLayoutCreateInfo);
        if (!descriptorSetLayout)
        {
            throw std::runtime_error("Failed to create descriptor set layout");
        }
        mDescriptorSetLayouts.push_back(std::move(descriptorSetLayout));
    }
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    descriptorSetLayouts.reserve(mDescriptorSetLayouts.size());
    for (auto &layout : mDescriptorSetLayouts)
    {
        descriptorSetLayouts.push_back(layout.get());
    }
    pipelineLayoutCreateInfo.setFlags(mPipelineLayoutDesc.Flags).setSetLayouts(descriptorSetLayouts);
    pipelineLayoutCreateInfo.setPushConstantRanges(mPipelineLayoutDesc.PushConstantRanges);
    mPipelineLayout = RHIContext::Instance().GetDevice().createPipelineLayoutUnique(pipelineLayoutCreateInfo);
    if (!mPipelineLayout)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}
} // namespace MEngine::Platform