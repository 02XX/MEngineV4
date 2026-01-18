#include "GraphicPipelineBuilderLightingOpaquePBR.hpp"
#include "PBRMaterialManager.hpp"
#include "ShaderManager.hpp"

namespace MEngine::Resource
{
void GraphicPipelineBuilderLightingOpaquePBR::SetShader()
{
    // vertex
    auto lightingVertShader = mShaderManager->GetByName(DefaultShaderType::LightingOpaquePBRVert);
    if (!lightingVertShader)
    {
        LogError("GraphicPipelineBuilderLightingOpaquePBR: lighting_vert shader not found in ShaderManager");
        return;
    }
    lightingVertShader->GetResource()->InitResource(mContext);

    mShaders.push_back(lightingVertShader);
    // fragment
    auto lightingFragShader = mShaderManager->GetByName(DefaultShaderType::LightingOpaquePBRFrag);
    if (!lightingFragShader)
    {
        LogError("GraphicPipelineBuilderLightingOpaquePBR: lighting_frag shader not found in ShaderManager");
        return;
    }
    lightingFragShader->GetResource()->InitResource(mContext);

    mShaders.push_back(lightingFragShader);
}
void GraphicPipelineBuilderLightingOpaquePBR::SetColorBlendState()
{
    // color
    mColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
}
void GraphicPipelineBuilderLightingOpaquePBR::SetColorAttachmentFormats()
{
    mColorAttachmentFormats = {
        vk::Format::eR8G8B8A8Srgb, // color
    };
}

void GraphicPipelineBuilderLightingOpaquePBR::SetName()
{
    mName = DefaultGraphicPipelineType::LightingOpaquePBR;
}
} // namespace MEngine::Resource