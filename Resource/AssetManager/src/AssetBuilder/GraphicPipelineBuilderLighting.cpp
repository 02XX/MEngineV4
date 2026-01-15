#include "GraphicPipelineBuilderLighting.hpp"

namespace MEngine::Resource
{
void GraphicPipelineBuilderLighting::SetShader()
{
    // vertex
    auto lightingVertShader = mShaderManager->GetByName("lighting_vert");
    if (!lightingVertShader)
    {
        LogError("GraphicPipelineBuilderLighting: lighting_vert shader not found in ShaderManager");
        return;
    }
    lightingVertShader->GetResource()->InitResource(mContext);

    mShaders.push_back(lightingVertShader);
    // fragment
    auto lightingFragShader = mShaderManager->GetByName("lighting_frag");
    if (!lightingFragShader)
    {
        LogError("GraphicPipelineBuilderLighting: lighting_frag shader not found in ShaderManager");
        return;
    }
    lightingFragShader->GetResource()->InitResource(mContext);

    mShaders.push_back(lightingFragShader);
}
void GraphicPipelineBuilderLighting::SetColorBlendState()
{
    // color
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
}
void GraphicPipelineBuilderLighting::SetColorAttachmentFormats()
{
    mColorAttachmentFormats = {
        vk::Format::eR8G8B8A8Srgb, // color
    };
}
void GraphicPipelineBuilderLighting::SetLayout()
{
    GraphicPipelineBuilder::SetLayout();
    // TODO: bindless descriptor sets
    //  mPipelineLayoutDesc.DescriptorSetLayoutBindings.push_back({
    //      // set:1
    //      // Binding: 0 PBR Parameters
    //      vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
    //      vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 1 Albedo Map
    //      vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eInputAttachment, 1,
    //      vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 2 Normal Map
    //      vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eInputAttachment, 1,
    //      vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 3 ARM Map
    //      vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eInputAttachment, 1,
    //      vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 4 Emissive Map
    //      vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eInputAttachment, 1,
    //      vk::ShaderStageFlagBits::eFragment},
    //  });
    //  mPipelineLayoutDesc.PushConstantRanges.push_back(
    //      vk::PushConstantRange()
    //          .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
    //          .setOffset(0)
    //          .setSize(sizeof(Matrix4)));
}
void GraphicPipelineBuilderLighting::SetName()
{
    mName = "GraphicPipeline_Lighting";
}
} // namespace MEngine::Resource