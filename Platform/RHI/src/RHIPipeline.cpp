#include "RHIPipeline.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHIPipeline::RHIPipeline(const RHIPipelineLayoutDesc &layoutDesc) : RHIResource(), mPipelineLayoutDesc(layoutDesc)
{
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.setFlags(mPipelineLayoutDesc.mFlags)
        .setSetLayouts(mPipelineLayoutDesc.mDescriptorSetLayouts)
        .setPushConstantRanges(mPipelineLayoutDesc.mPushConstantRanges);
    mPipelineLayout = RHIContext::Instance().GetDevice().createPipelineLayoutUnique(pipelineLayoutCreateInfo);
    if (!mPipelineLayout)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}
} // namespace MEngine::Platform