#pragma once
#include "RHIPipeline.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
struct RHIGraphicPipelineDesc
{
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
    vk::RenderPass mRenderPass{};
    uint32_t mSubPass{0};
};

class RHIGraphicPipeline : public RHIPipeline
{
  protected:
    RHIGraphicPipelineDesc mGraphicPipelineDesc{};

  public:
    RHIGraphicPipeline(const RHIGraphicPipelineDesc &graphicPipelineDesc,
                       const RHIPipelineLayoutDesc &graphicPipelinelayoutDesc);
    ~RHIGraphicPipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
    inline vk::PipelineLayout GetPipelineLayout() const
    {
        return mPipelineLayout.get();
    }
};
} // namespace MEngine::Platform