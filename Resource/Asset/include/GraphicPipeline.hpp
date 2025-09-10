#pragma once
#include "Pipeline.hpp"
#include "RenderPassType.hpp"
using namespace MEngine::Core;
namespace MEngine::Function
{
class GraphicPipelineBuilder;
class GBufferPipelineBuilder;
class CustomGraphicPipelineBuilder;
class LightingPipelineBuilder;
} // namespace MEngine::Function
namespace MEngine::Resource
{
class GraphicPipeline final : public Pipeline
{
    friend class Function::GraphicPipelineBuilder;
    friend class Function::GBufferPipelineBuilder;
    friend class Function::CustomGraphicPipelineBuilder;
    friend class Function::LightingPipelineBuilder;

  protected:
    vk::GraphicsPipelineCreateInfo mCreateInfo{};
    vk::VertexInputBindingDescription mVertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::PipelineVertexInputStateCreateInfo mVertexInputInfo{};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};
    vk::PipelineRasterizationStateCreateInfo mRasterizationState{};
    vk::PipelineViewportStateCreateInfo mViewportState{};
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    vk::PipelineDynamicStateCreateInfo mDynamicState{};
    std::vector<vk::DynamicState> mDynamicStates{};

    std::vector<vk::PipelineShaderStageCreateInfo> mShaderStages{};
    RenderPassType mRenderPassType{RenderPassType::ForwardOpaque};

  protected:
    GraphicPipeline() : Pipeline()
    {
    }

  public:
    ~GraphicPipeline() override = default;
};
} // namespace MEngine::Resource