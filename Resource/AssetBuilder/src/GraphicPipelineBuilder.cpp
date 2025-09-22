#include "GraphicPipelineBuilder.hpp"
#include "GraphicPipeline.hpp"
#include "Logger.hpp"
#include <memory>
#include <vector>

namespace MEngine::Resource
{
void GraphicPipelineBuilder::Reset()
{
    mGraphicPipelineState = {};
    mPipelineLayoutDesc = {};
}
std::unique_ptr<GraphicPipeline> GraphicPipelineBuilder::Build()
{
    mGraphicPipelineState.ColorBlendState.setAttachments(mGraphicPipelineState.ColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f})
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy);
    mGraphicPipeline = std::make_unique<GraphicPipeline>(mName, mGraphicPipelineState, mPipelineLayoutDesc);
    if (!mGraphicPipeline)
    {
        LogError("Failed to create Graphic Pipeline");
    }
    return std::move(mGraphicPipeline);
}
void GraphicPipelineBuilder::SetInputAssemblyState()
{
    mGraphicPipelineState.InputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList)
        .setPrimitiveRestartEnable(vk::False);
}

void GraphicPipelineBuilder::SetRasterizationState()
{
    mGraphicPipelineState.RasterizationState.setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setLineWidth(1.0f)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False);
}
void GraphicPipelineBuilder::SetMultiSampleState()
{
    mGraphicPipelineState.MultisampleState.setSampleShadingEnable(vk::False)
        .setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setMinSampleShading(1.0f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(vk::False)
        .setAlphaToOneEnable(vk::False);
}
void GraphicPipelineBuilder::SetDepthStencilState()
{
    mGraphicPipelineState.DepthStencilState.setDepthTestEnable(vk::True)
        .setDepthWriteEnable(vk::True)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setMinDepthBounds(0.0f)
        .setMaxDepthBounds(1.0f)
        .setStencilTestEnable(vk::False);
}
void GraphicPipelineBuilder::SetColorBlendState()
{
    mGraphicPipelineState.ColorBlendAttachments = {
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False)};
    mGraphicPipelineState.ColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mGraphicPipelineState.ColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GraphicPipelineBuilder::SetColorAttachmentFormats()
{
    mGraphicPipelineState.ColorAttachmentFormats = {vk::Format::eR32G32B32A32Sfloat};
}
void GraphicPipelineBuilder::SetDepthStencilAttachmentFormat()
{
    mGraphicPipelineState.DepthStencilAttachmentFormat = vk::Format::eD32SfloatS8Uint;
}
void GraphicPipelineBuilder::SetLayout()
{
    mPipelineLayoutDesc.DescriptorSetLayoutBindings.push_back({
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