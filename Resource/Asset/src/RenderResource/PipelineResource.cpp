#include "PipelineResource.hpp"
#include "Pipeline.hpp"
namespace MEngine::Resource
{
PipelineResource::PipelineResource(Pipeline *pipeline) : RenderResource(pipeline)
{
}
void PipelineResource::InitRHI(std::shared_ptr<Context> context)
{
    auto pipeline = static_cast<Pipeline *>(mOwnerAsset);
    auto device = context->Device.get();
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.setFlags(vk::PipelineLayoutCreateFlags())
        .setSetLayouts(context->DescriptorSetLayout.get());
    pipelineLayoutCreateInfo.setPushConstantRanges(pipeline->mPushConstantRanges);
    mPipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);
    if (!mPipelineLayout)
    {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}
void PipelineResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    if (mPipelineLayout)
        device.destroyPipelineLayout(mPipelineLayout);
    if (mPipeline)
        device.destroyPipeline(mPipeline);
}
} // namespace MEngine::Resource