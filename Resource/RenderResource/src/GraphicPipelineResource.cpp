#include "GraphicPipelineResource.hpp"
#include "PipelineResource.hpp"
namespace MEngine::Resource
{

void GraphicPipelineResource::InitRHI(std::shared_ptr<Context> context)
{
    PipelineResource::InitRHI(context);

    auto device = context->Device.get();
    vk::PipelineRenderingCreateInfo pipelineRenderingInfo{};
    pipelineRenderingInfo.setColorAttachmentCount(static_cast<uint32_t>(mColorBlendAttachments.size()))
        .setColorAttachmentFormats(mColorAttachmentFormats)
        .setDepthAttachmentFormat(mDepthStencilAttachmentFormat)
        .setStencilAttachmentFormat(mDepthStencilAttachmentFormat);

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptions(mVertexBindings).setVertexAttributeDescriptions(mVertexAttributes);

    vk::PipelineViewportStateCreateInfo viewportState{};
    vk::Viewport viewport{};
    vk::Rect2D scissor{};
    viewportState.setViewports(viewport).setScissors(scissor);
    std::vector<vk::DynamicState> dynamicStates{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.setDynamicStates(dynamicStates);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &shader : mShaders)
    {
        shaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                   .setStage(shader->GetShaderStage())
                                   .setModule(shader->GetShaderModule())
                                   .setPName("main"));
    }

    mColorBlendState.setAttachments(mColorBlendAttachments);

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.setPVertexInputState(&vertexInputInfo)
        .setPInputAssemblyState(&mInputAssemblyState)
        .setPRasterizationState(&mRasterizationState)
        .setPViewportState(&viewportState)
        .setPMultisampleState(&mMultisampleState)
        .setPDepthStencilState(&mDepthStencilState)
        .setPColorBlendState(&mColorBlendState)
        .setPDynamicState(&dynamicStateInfo)
        .setStages(shaderStages)
        .setLayout(mPipelineLayout)
        .setPNext(&pipelineRenderingInfo);
    auto result = device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
    mPipeline = result.value;
};
} // namespace MEngine::Resource