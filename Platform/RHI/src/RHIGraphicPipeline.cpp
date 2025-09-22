#include "RHIGraphicPipeline.hpp"
#include "RHIContext.hpp"
#include <vector>
namespace MEngine::Platform
{
RHIGraphicPipeline::RHIGraphicPipeline(const RHIGraphicPipelineDesc &graphicPipelineDesc,
                                       const RHIPipelineLayoutDesc &graphicPipelinelayoutDesc)
    : RHIPipeline(graphicPipelinelayoutDesc), mGraphicPipelineDesc(graphicPipelineDesc)
{
    vk::PipelineRenderingCreateInfo pipelineRenderingInfo{};
    pipelineRenderingInfo
        .setColorAttachmentCount(static_cast<uint32_t>(mGraphicPipelineDesc.ColorBlendAttachments.size()))
        .setColorAttachmentFormats(mGraphicPipelineDesc.ColorAttachmentFormats)
        .setDepthAttachmentFormat(mGraphicPipelineDesc.DepthStencilAttachmentFormat)
        .setStencilAttachmentFormat(mGraphicPipelineDesc.DepthStencilAttachmentFormat);
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptions(mGraphicPipelineDesc.VertexBindings)
        .setVertexAttributeDescriptions(mGraphicPipelineDesc.VertexAttributes);
    vk::PipelineViewportStateCreateInfo viewportState{};
    vk::Viewport viewport{};
    viewport.setX(0.0f).setY(0.0f).setWidth(800.0f).setHeight(600.0f).setMinDepth(0.0f).setMaxDepth(1.0f);
    vk::Rect2D scissor{};
    scissor.setOffset({0, 0}).setExtent({800, 600});
    viewportState.setViewports(viewport).setScissors(scissor);
    std::vector<vk::DynamicState> dynamicStates{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.setDynamicStates(dynamicStates);
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &shader : mGraphicPipelineDesc.Shaders)
    {
        shaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                   .setStage(shader->GetShaderStage())
                                   .setModule(shader->GetShaderModule())
                                   .setPName("main"));
    }
    mGraphicPipelineDesc.ColorBlendState.setAttachments(mGraphicPipelineDesc.ColorBlendAttachments);
    pipelineCreateInfo.setStages(shaderStages)
        .setPVertexInputState(&vertexInputInfo)
        .setPInputAssemblyState(&mGraphicPipelineDesc.InputAssemblyState)
        .setPRasterizationState(&mGraphicPipelineDesc.RasterizationState)
        .setPViewportState(&viewportState)
        .setPMultisampleState(&mGraphicPipelineDesc.MultisampleState)
        .setPDepthStencilState(&mGraphicPipelineDesc.DepthStencilState)
        .setPColorBlendState(&mGraphicPipelineDesc.ColorBlendState)
        .setPDynamicState(&dynamicStateInfo)
        .setLayout(mPipelineLayout.get())
        .setPNext(&pipelineRenderingInfo);
    auto result = RHIContext::Instance().GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
    mPipeline = std::move(result.value);
}
} // namespace MEngine::Platform