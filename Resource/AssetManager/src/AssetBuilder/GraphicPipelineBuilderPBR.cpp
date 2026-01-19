#include "GraphicPipelineBuilderPBR.hpp"
#include "Math.hpp"

namespace MEngine::Resource
{
void GraphicPipelineBuilderPBR::SetLayout()
{
    GraphicPipelineBuilder::SetLayout();
    mDescriptorSetLayouts.push_back(
        mContext->DefaultDescriptorSetLayouts[Context::DefaultDescriptorSetLayoutType::PBR].get());
    mPushConstantRanges.push_back(
        vk::PushConstantRange()
            .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
            .setOffset(0)
            .setSize(sizeof(Matrix4))); // ModelMatrix
}
void GraphicPipelineBuilderPBR::SetName()
{
    mName = "GraphicPipeline_PBR";
}
} // namespace MEngine::Resource