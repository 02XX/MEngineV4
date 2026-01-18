#include "GraphicPipelineBuilderPBR.hpp"
#include "PBRMaterialResource.hpp"

namespace MEngine::Resource
{
void GraphicPipelineBuilderPBR::SetLayout()
{
    mPushConstantRanges.push_back(
        vk::PushConstantRange()
            .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
            .setOffset(0)
            .setSize(sizeof(PBRMaterialPushConstants)));
}
void GraphicPipelineBuilderPBR::SetName()
{
    mName = "GraphicPipeline_PBR";
}
} // namespace MEngine::Resource