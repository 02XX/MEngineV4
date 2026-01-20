#include "GraphicPipelineBuilderPhong.hpp"
#include "Math.hpp"

namespace MEngine::Resource
{
void GraphicPipelineBuilderPhong::SetLayout()
{
    GraphicPipelineBuilder::SetLayout();
    mDescriptorSetLayouts.push_back(
        mContext->DefaultDescriptorSetLayouts[Context::DefaultDescriptorSetLayoutType::Phong].get());
    mPushConstantRanges.push_back(
        vk::PushConstantRange()
            .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
            .setOffset(0)
            .setSize(sizeof(Matrix4))); // ModelMatrix
}
void GraphicPipelineBuilderPhong::SetName()
{
    mName = "GraphicPipeline_Phong";
}
} // namespace MEngine::Resource