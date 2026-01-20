#include "GraphicPipelineBuilderForwardOpaquePhong.hpp"
#include "GraphicPipelineManager.hpp"
#include "PhongMaterialResource.hpp"
#include "ShaderManager.hpp"
namespace MEngine::Resource
{
void GraphicPipelineBuilderForwardOpaquePhong::SetShader()
{
    // vertex
    auto forwardVertShader = mShaderManager->GetByName(DefaultShaderType::ForwardOpaquePhongVert);
    if (!forwardVertShader)
    {
        LogError("GraphicPipelineBuilderForwardOpaquePhong: forward_vert shader not found in ShaderManager");
        return;
    }
    forwardVertShader->GetResource()->InitResource(mContext);
    mShaders.push_back(forwardVertShader);
    // fragment
    auto forwardFragShader = mShaderManager->GetByName(DefaultShaderType::ForwardOpaquePhongFrag);
    if (!forwardFragShader)
    {
        LogError("GraphicPipelineBuilderForwardOpaquePhong: forward_frag shader not found in ShaderManager");
        return;
    }
    forwardFragShader->GetResource()->InitResource(mContext);
    mShaders.push_back(forwardFragShader);
}
void GraphicPipelineBuilderForwardOpaquePhong::SetColorBlendState()
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

    mColorBlendState.setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(mColorBlendAttachments)
        .setBlendConstants({0.0f, 0.0f, 0.0f, 0.0f});
}
void GraphicPipelineBuilderForwardOpaquePhong::SetColorAttachmentFormats()
{
    mColorAttachmentFormats.clear();
    mColorAttachmentFormats = {
        vk::Format::eR32G32B32A32Sfloat, // color
    };
}
void GraphicPipelineBuilderForwardOpaquePhong::SetName()
{
    mName = DefaultGraphicPipelineType::ForwardOpaquePhong;
}
} // namespace MEngine::Resource