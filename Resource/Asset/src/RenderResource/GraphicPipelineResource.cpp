#include "GraphicPipelineResource.hpp"
#include "GraphicPipeline.hpp"
#include "PipelineResource.hpp"
namespace MEngine::Resource
{
GraphicPipelineResource::GraphicPipelineResource(GraphicPipeline *pipeline) : PipelineResource(pipeline)
{
}
void GraphicPipelineResource::InitRHI(std::shared_ptr<Context> context)
{
    PipelineResource::InitRHI(context);
    auto pipeline = static_cast<GraphicPipeline *>(mOwnerAsset);
    auto device = context->Device.get();
    vk::PipelineRenderingCreateInfo pipelineRenderingInfo{};
    pipelineRenderingInfo.setColorAttachmentCount(static_cast<uint32_t>(pipeline->mColorBlendAttachments.size()))
        .setColorAttachmentFormats(pipeline->mColorAttachmentFormats)
        .setDepthAttachmentFormat(pipeline->mDepthStencilAttachmentFormat)
        .setStencilAttachmentFormat(pipeline->mDepthStencilAttachmentFormat);

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptions(pipeline->mVertexBindings)
        .setVertexAttributeDescriptions(pipeline->mVertexAttributes);

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
    for (const auto &shader : pipeline->mShaders)
    {
        auto shaderResource = shader->GetResourceAs<ShaderResource>();
        shaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                   .setStage(shader->GetShaderStage())
                                   .setModule(shaderResource->GetShaderModule())
                                   .setPName("main"));
    }

    pipeline->mColorBlendState.setAttachments(pipeline->mColorBlendAttachments);

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.setPVertexInputState(&vertexInputInfo)
        .setPInputAssemblyState(&pipeline->mInputAssemblyState)
        .setPRasterizationState(&pipeline->mRasterizationState)
        .setPViewportState(&viewportState)
        .setPMultisampleState(&pipeline->mMultisampleState)
        .setPDepthStencilState(&pipeline->mDepthStencilState)
        .setPColorBlendState(&pipeline->mColorBlendState)
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