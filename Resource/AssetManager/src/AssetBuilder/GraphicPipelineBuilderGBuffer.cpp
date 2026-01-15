#include "GraphicPipelineBuilderGBuffer.hpp"

namespace MEngine::Resource
{
void GraphicPipelineBuilderGBuffer::SetShader()
{
    // vertex
    auto gBufferVertShader = mShaderManager->GetByName("gbuffer_vert");
    if (!gBufferVertShader)
    {
        LogError("GraphicPipelineBuilderGBuffer: gbuffer_vert shader not found in ShaderManager");
        return;
    }
    gBufferVertShader->GetResource()->InitResource(mContext);
    mShaders.push_back(gBufferVertShader);
    // fragment
    auto gBufferFragShader = mShaderManager->GetByName("gbuffer_frag");
    if (!gBufferFragShader)
    {
        LogError("GraphicPipelineBuilderGBuffer: gbuffer_frag shader not found in ShaderManager");
        return;
    }
    gBufferFragShader->GetResource()->InitResource(mContext);
    mShaders.push_back(gBufferFragShader);
}
void GraphicPipelineBuilderGBuffer::SetColorBlendState()
{
    mColorBlendAttachments.clear();
    // color
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd));
    // albedo
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd));
    // normal
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd));
    // arm
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd));
    // position
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd));
    // emissive
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd));

    mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GraphicPipelineBuilderGBuffer::SetColorAttachmentFormats()
{
    mColorAttachmentFormats.clear();
    mColorAttachmentFormats = {
        vk::Format::eR8G8B8A8Srgb,       // color
        vk::Format::eR32G32B32A32Sfloat, // albedo
        vk::Format::eR32G32B32A32Sfloat, // normal
        vk::Format::eR32G32B32A32Sfloat, // arm
        vk::Format::eR32G32B32A32Sfloat, // position
        vk::Format::eR32G32B32A32Sfloat  // emissive
    };
}
void GraphicPipelineBuilderGBuffer::SetLayout()
{
    GraphicPipelineBuilder::SetLayout();
    // TODO: bindless descriptor sets

    //  mDescriptorSetLayoutBindings.push_back({
    //      // set:1
    //      // Binding: 0 PBR Parameters
    //      vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
    //                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 1 Albedo Map
    //      vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eCombinedImageSampler, 1,
    //                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 2 Normal Map
    //      vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
    //                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 3 ARM Map
    //      vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
    //                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    //      // Binding: 4 Emissive Map
    //      vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
    //                                     vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    //  });
    //  mPushConstantRanges.push_back(
    //      vk::PushConstantRange()
    //          .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
    //          .setOffset(0)
    //          .setSize(sizeof(Matrix4)));
}
void GraphicPipelineBuilderGBuffer::SetName()
{
    mName = "GraphicPipeline_GBuffer";
}
} // namespace MEngine::Resource