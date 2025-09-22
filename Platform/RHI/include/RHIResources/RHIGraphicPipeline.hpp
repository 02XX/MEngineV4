#pragma once
#include "RHIHandler.hpp"
#include "RHIPipeline.hpp"
#include "RHIShader.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
struct RHIGraphicPipelineDesc
{
    vk::VertexInputBindingDescription VertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> VertexAttributes{};
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyState{};

    vk::PipelineRasterizationStateCreateInfo RasterizationState{};
    vk::PipelineMultisampleStateCreateInfo MultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo DepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo ColorBlendState{};

    std::vector<RHIHandler<RHIShader>> Shaders{};

    std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachments{};
    std::vector<vk::Format> ColorAttachmentFormats{};
    vk::Format DepthStencilAttachmentFormat{vk::Format::eD32Sfloat};
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