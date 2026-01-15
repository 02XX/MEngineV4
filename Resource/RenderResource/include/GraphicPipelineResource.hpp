#pragma once

#include "Context.hpp"
#include "PipelineResource.hpp"
#include "ShaderResource.hpp"
namespace MEngine::Resource
{
class GraphicPipelineResource final : public PipelineResource
{
  protected:
    std::vector<ShaderResource *>
        mShaders{}; // RenderResource 不应该持有其他RenderResource的所有权，而是由上层的Asset来管理所有权
    vk::VertexInputBindingDescription mVertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};
    vk::PipelineRasterizationStateCreateInfo mRasterizationState{};
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    std::vector<vk::Format> mColorAttachmentFormats{};
    vk::Format mDepthStencilAttachmentFormat{};

  public:
    GraphicPipelineResource(std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings,
                            std::vector<vk::PushConstantRange> pushConstantRanges,
                            std::vector<ShaderResource *> shaders, vk::VertexInputBindingDescription vertexBindings,
                            std::vector<vk::VertexInputAttributeDescription> vertexAttributes,
                            vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState,
                            vk::PipelineRasterizationStateCreateInfo rasterizationState,
                            vk::PipelineMultisampleStateCreateInfo multisampleState,
                            vk::PipelineDepthStencilStateCreateInfo depthStencilState,
                            vk::PipelineColorBlendStateCreateInfo colorBlendState,
                            std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments,
                            std::vector<vk::Format> colorAttachmentFormats, vk::Format depthStencilAttachmentFormat)
        : PipelineResource(descriptorSetLayoutBindings, pushConstantRanges), mVertexBindings(vertexBindings),
          mVertexAttributes(vertexAttributes), mInputAssemblyState(inputAssemblyState),
          mRasterizationState(rasterizationState), mMultisampleState(multisampleState),
          mDepthStencilState(depthStencilState), mColorBlendState(colorBlendState), mShaders(shaders),
          mColorBlendAttachments(colorBlendAttachments), mColorAttachmentFormats(colorAttachmentFormats),
          mDepthStencilAttachmentFormat(depthStencilAttachmentFormat) {};
    ~GraphicPipelineResource() override = default;

    void InitRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource