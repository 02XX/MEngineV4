#include "GraphicPipelineBuilder.hpp"
#include "Vertex.hpp"
namespace MEngine::Resource
{
void GraphicPipelineBuilder::Reset()
{
    mDescriptorSetLayoutBindings.clear();
    mPushConstantRanges.clear();

    mShaders.clear();
    mVertexBindings = vk::VertexInputBindingDescription{};
    mVertexAttributes.clear();
    mInputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{};
    mRasterizationState = vk::PipelineRasterizationStateCreateInfo{};
    mMultisampleState = vk::PipelineMultisampleStateCreateInfo{};
    mDepthStencilState = vk::PipelineDepthStencilStateCreateInfo{};
    mColorBlendState = vk::PipelineColorBlendStateCreateInfo{};
    mColorBlendAttachments.clear();
    mColorAttachmentFormats.clear();
    mDepthStencilAttachmentFormat = vk::Format::eUndefined;
}
std::unique_ptr<GraphicPipeline> GraphicPipelineBuilder::Build()
{
    mColorBlendState.setAttachments(mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f})
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy);
    auto graphicPipeline = std::make_unique<GraphicPipeline>(
        mName, mDescriptorSetLayoutBindings, mPushConstantRanges, mShaders, mVertexBindings, mVertexAttributes,
        mInputAssemblyState, mRasterizationState, mMultisampleState, mDepthStencilState, mColorBlendState,
        mColorBlendAttachments, mColorAttachmentFormats, mDepthStencilAttachmentFormat);
    if (!graphicPipeline)
    {
        throw std::runtime_error("Failed to build GraphicPipeline");
    }
    return graphicPipeline;
}
void GraphicPipelineBuilder::SetVertexInputState()
{
    mVertexAttributes = Vertex::GetVertexInputAttributeDescription();
    mVertexBindings = Vertex::GetVertexInputBindingDescription();
}
void GraphicPipelineBuilder::SetInputAssemblyState()
{
    mInputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList).setPrimitiveRestartEnable(vk::False);
}

void GraphicPipelineBuilder::SetRasterizationState()
{
    mRasterizationState.setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False);
}
void GraphicPipelineBuilder::SetMultiSampleState()
{
    mMultisampleState.setSampleShadingEnable(vk::False)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(vk::False)
        .setAlphaToOneEnable(vk::False);
}
void GraphicPipelineBuilder::SetDepthStencilState()
{
    mDepthStencilState.setDepthTestEnable(vk::True)
        .setDepthWriteEnable(vk::True)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setMinDepthBounds(0.0f)
        .setMaxDepthBounds(1.0f)
        .setStencilTestEnable(vk::False);
}
void GraphicPipelineBuilder::SetColorBlendState()
{
    mColorBlendAttachments = {vk::PipelineColorBlendAttachmentState()
                                  .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                     vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
                                  .setBlendEnable(vk::False)};
    mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GraphicPipelineBuilder::SetColorAttachmentFormats()
{
    mColorAttachmentFormats = {vk::Format::eR32G32B32A32Sfloat};
}
void GraphicPipelineBuilder::SetDepthStencilAttachmentFormat()
{
    mDepthStencilAttachmentFormat = vk::Format::eD32SfloatS8Uint;
}
void GraphicPipelineBuilder::SetLayout()
{
    mDescriptorSetLayoutBindings.push_back({
        // set:0
        // Binding: 0 VP (View Projection Matrix)
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Light
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eUniformBuffer, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Environment Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 Irradiance Map
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 BRDF LUT
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment},
    });
}
} // namespace MEngine::Resource