#include "LightingPipelineBuilder.hpp"
#include "GraphicPipelineBuilder.hpp"
#include "Math.hpp"
#include <memory>
#include <vector>

namespace MEngine::Resource
{
void LightingPipelineBuilder::SetShader()
{
    // vertex
    auto lightingVertShader = mShaderManager->GetByName("lighting_vert");
    if (!lightingVertShader)
    {
        LogError("LightingPipelineBuilder: lighting_vert shader not found in ShaderManager");
        return;
    }
    lightingVertShader->GetResource()->InitResource();

    mGraphicPipelineState.Shaders.push_back(lightingVertShader);
    // fragment
    auto lightingFragShader = mShaderManager->GetByName("lighting_frag");
    if (!lightingFragShader)
    {
        LogError("LightingPipelineBuilder: lighting_frag shader not found in ShaderManager");
        return;
    }
    lightingFragShader->GetResource()->InitResource();

    mGraphicPipelineState.Shaders.push_back(lightingFragShader);
}
void LightingPipelineBuilder::SetColorBlendState()
{
    // color
    mGraphicPipelineState.ColorBlendAttachments.push_back(
        vk::PipelineColorBlendAttachmentState()
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(vk::False));
}
void LightingPipelineBuilder::SetColorAttachmentFormats()
{
    mGraphicPipelineState.ColorAttachmentFormats = {
        vk::Format::eR8G8B8A8Srgb, // color
    };
}
void LightingPipelineBuilder::SetLayout()
{
    GraphicPipelineBuilder::SetLayout();
    mPipelineLayoutDesc.DescriptorSetLayoutBindings.push_back({
        // set:1
        // Binding: 0 PBR Parameters
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eFragment},
        // Binding: 1 Albedo Map
        vk::DescriptorSetLayoutBinding{1, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment},
        // Binding: 2 Normal Map
        vk::DescriptorSetLayoutBinding{2, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment},
        // Binding: 3 Metallic Map
        vk::DescriptorSetLayoutBinding{3, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment},
        // Binding: 4 Roughness Map
        vk::DescriptorSetLayoutBinding{4, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment},
        // Binding: 5 AO Map
        vk::DescriptorSetLayoutBinding{5, vk::DescriptorType::eInputAttachment, 1, vk::ShaderStageFlagBits::eFragment},
    });
    mPipelineLayoutDesc.PushConstantRanges.push_back(
        vk::PushConstantRange()
            .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment)
            .setOffset(0)
            .setSize(sizeof(Matrix4)));
}
void LightingPipelineBuilder::SetName()
{
    mName = "LightingPipeline";
}
} // namespace MEngine::Resource