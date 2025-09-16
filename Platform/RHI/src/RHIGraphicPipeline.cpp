#include "RHIGraphicPipeline.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHIGraphicPipeline::RHIGraphicPipeline(const RHIGraphicPipelineDesc &graphicPipelineDesc,
                                       const RHIPipelineLayoutDesc &graphicPipelinelayoutDesc)
    : RHIPipeline(graphicPipelinelayoutDesc), mGraphicPipelineDesc(graphicPipelineDesc)
{
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.setStages(mGraphicPipelineDesc.mShaderStages)
        .setPVertexInputState(&mGraphicPipelineDesc.mVertexInputInfo)
        .setPInputAssemblyState(&mGraphicPipelineDesc.mInputAssemblyState)
        .setPRasterizationState(&mGraphicPipelineDesc.mRasterizationState)
        .setPViewportState(&mGraphicPipelineDesc.mViewportState)
        .setPMultisampleState(&mGraphicPipelineDesc.mMultisampleState)
        .setPDepthStencilState(&mGraphicPipelineDesc.mDepthStencilState)
        .setPColorBlendState(&mGraphicPipelineDesc.mColorBlendState)
        .setPDynamicState(&mGraphicPipelineDesc.mDynamicState)
        .setLayout(mPipelineLayout.get())
        .setRenderPass(mGraphicPipelineDesc.mRenderPass)
        .setSubpass(mGraphicPipelineDesc.mSubPass);
    auto result = RHIContext::Instance().GetDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
    mPipeline = std::move(result.value);
}
} // namespace MEngine::Platform