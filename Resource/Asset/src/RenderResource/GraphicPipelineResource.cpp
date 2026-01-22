#include "GraphicPipelineResource.hpp"
#include "GraphicPipeline.hpp"
#include "PipelineResource.hpp"
#include <vulkan/vulkan_enums.hpp>
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
    viewportState.setViewports(viewport).setScissors(scissor).setViewportCount(0).setScissorCount(0);
    std::vector<vk::DynamicState> dynamicStates{
        // =============顶点输入=========================
        //==============================================
        // =============光栅化=========================
        vk::DynamicState::eCullMode,
        vk::DynamicState::eFrontFace,
        vk::DynamicState::eLineWidth,
        vk::DynamicState::eDepthBiasEnable,
        vk::DynamicState::eDepthBias,
        // 深度模板状态
        vk::DynamicState::eDepthTestEnable,
        vk::DynamicState::eDepthWriteEnable,
        vk::DynamicState::eDepthCompareOp,
        vk::DynamicState::eStencilTestEnable,
        vk::DynamicState::eStencilOp,
        //==============================================
        // =============多重采样=========================
        //==============================================
        //=============视口和剪裁====================
        vk::DynamicState::eViewportWithCount,
        vk::DynamicState::eScissorWithCount,
        //==============================================
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateInfo{

    };
    dynamicStateInfo.setDynamicStates(dynamicStates);

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    for (const auto &shader : pipeline->mShaders)
    {
        auto shaderResource = shader->GetResourceAs<ShaderResource>();
        shaderResource->InitResource(context);
        shaderStages.push_back(vk::PipelineShaderStageCreateInfo{}
                                   .setStage(shader->mStage)
                                   .setModule(shaderResource->GetShaderModule())
                                   .setPName(shader->mEntryPoint.c_str()));
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