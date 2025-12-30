#include "GraphicPipelineResource.hpp"
#include "GraphicPipeline.hpp"
#include "RHIHandler.hpp"
#include "RHIPipeline.hpp"
#include "Vertex.hpp"
#include <vector>
namespace MEngine::Resource
{
GraphicPipelineResource::GraphicPipelineResource(GraphicPipeline *graphicPipeline)
    : RenderResource(), mGraphicPipeline(graphicPipeline)
{
}

void GraphicPipelineResource::InitRHI()
{
    RHIGraphicPipelineDesc graphicPipelineDesc{};
    graphicPipelineDesc.InputAssemblyState = mGraphicPipeline->mGraphicPipelineState.InputAssemblyState;
    graphicPipelineDesc.RasterizationState = mGraphicPipeline->mGraphicPipelineState.RasterizationState;
    graphicPipelineDesc.MultisampleState = mGraphicPipeline->mGraphicPipelineState.MultisampleState;
    graphicPipelineDesc.DepthStencilState = mGraphicPipeline->mGraphicPipelineState.DepthStencilState;
    graphicPipelineDesc.ColorBlendState = mGraphicPipeline->mGraphicPipelineState.ColorBlendState;
    std::vector<RHIHandler<RHIShader>> shaders;
    for (auto shader : mGraphicPipeline->mGraphicPipelineState.Shaders)
    {
        auto shaderResource = static_cast<ShaderResource *>(shader->GetResource());
        shaderResource->InitResource();
        auto rhiShader = shaderResource->GetRHIShader();
        shaders.push_back(rhiShader);
    }
    graphicPipelineDesc.Shaders = std::move(shaders);
    graphicPipelineDesc.ColorBlendAttachments = mGraphicPipeline->mGraphicPipelineState.ColorBlendAttachments;
    graphicPipelineDesc.ColorAttachmentFormats = mGraphicPipeline->mGraphicPipelineState.ColorAttachmentFormats;
    graphicPipelineDesc.DepthStencilAttachmentFormat =
        mGraphicPipeline->mGraphicPipelineState.DepthStencilAttachmentFormat;
    auto vertexBindingDescriptions = Vertex::GetVertexInputBindingDescription();
    auto vertexAttributeDescriptions = Vertex::GetVertexInputAttributeDescription();
    graphicPipelineDesc.VertexBindings = vertexBindingDescriptions;
    graphicPipelineDesc.VertexAttributes = vertexAttributeDescriptions;
    RHIPipelineLayoutDesc RHIPipelineLayoutDesc{};
    RHIPipelineLayoutDesc.DescriptorSetLayoutBindings =
        mGraphicPipeline->mPipelineLayoutDesc.DescriptorSetLayoutBindings;
    RHIPipelineLayoutDesc.PushConstantRanges = mGraphicPipeline->mPipelineLayoutDesc.PushConstantRanges;
    RHIPipelineLayoutDesc.Flags = mGraphicPipeline->mPipelineLayoutDesc.Flags;
    mGraphicPipelineHandler =
        RHIHandler<RHIGraphicPipeline>(new RHIGraphicPipeline(graphicPipelineDesc, RHIPipelineLayoutDesc));

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        RHIDescriptorSetDesc desc{};
        desc.SetLayouts = {mGraphicPipelineHandler->GetDescriptorSetLayouts().front()};
        mPerFrameDescriptorSetHandlers[i] = RHIHandler<RHIDescriptorSet>(new RHIDescriptorSet(desc));
    }
};
void GraphicPipelineResource::ReleaseRHI()
{
    mGraphicPipelineHandler.SafeRelease();
}
} // namespace MEngine::Resource