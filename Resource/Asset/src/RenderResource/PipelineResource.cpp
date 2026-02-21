#include "PipelineResource.hpp"
#include "AssetManager.hpp"
#include "Pipeline.hpp"
namespace MEngine::Resource
{
PipelineResource::PipelineResource(Pipeline *pipeline) : RenderResource(pipeline)
{
}
void PipelineResource::InitRHI(std::shared_ptr<Context> context)
{
    // auto pipeline = static_cast<Pipeline *>(mOwnerAsset);
    // auto device = context->Device.get();
    // auto pipelineManager = std::dynamic_pointer_cast<PipelineManager>(AssetManager::Instance().GetManager<Pipeline>());
    // std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    // for (const auto &layoutName : pipeline->mDescriptorSetLayouts)
    // {
    //     descriptorSetLayouts.push_back(pipelineManager->mDefaultDescriptorSetLayouts.at(layoutName));
    // }
    // std::vector<vk::PushConstantRange> pushConstantRanges;
    // for (const auto &rangeName : pipeline->mPushConstantRanges)
    // {
    //     pushConstantRanges.push_back(pipelineManager->sDefaultPushConstantRanges.at(rangeName));
    // }
    // vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    // pipelineLayoutCreateInfo.setFlags(vk::PipelineLayoutCreateFlags())
    //     .setSetLayouts(descriptorSetLayouts)
    //     .setPushConstantRanges(pushConstantRanges);
    // mPipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);
    // if (!mPipelineLayout)
    // {
    //     throw std::runtime_error("Failed to create pipeline layout");
    // }
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