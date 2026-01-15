#pragma once
#include "Asset.hpp"
#include "GraphicPipelineResource.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "ShaderResource.hpp"
#include <memory>
#include <string>

namespace MEngine::Resource
{
class GraphicPipeline final : public Pipeline
{
    friend class GraphicPipelineResource;
    friend class GraphicPipelineBuilder;
    friend class GBufferPipelineBuilder;
    friend class GraphicPipelineManager;

  private:
    vk::VertexInputBindingDescription mVertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};

    vk::PipelineRasterizationStateCreateInfo mRasterizationState{};
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};

    std::vector<std::shared_ptr<Shader>>
        mShaders{}; // RenderResource 不应该持有其他RenderResource的所有权，而是由上层的Asset来管理所有权

    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    std::vector<vk::Format> mColorAttachmentFormats{};
    vk::Format mDepthStencilAttachmentFormat{};

  protected:
    GraphicPipeline() : Pipeline()
    {
    }

  public:
    GraphicPipeline(const std::string &name,
                    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> descriptorSetLayoutBindings,
                    std::vector<vk::PushConstantRange> pushConstantRanges,
                    vk::VertexInputBindingDescription vertexBindings,
                    std::vector<vk::VertexInputAttributeDescription> vertexAttributes,
                    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState,
                    vk::PipelineRasterizationStateCreateInfo rasterizationState,
                    vk::PipelineMultisampleStateCreateInfo multisampleState,
                    vk::PipelineDepthStencilStateCreateInfo depthStencilState,
                    vk::PipelineColorBlendStateCreateInfo colorBlendState, std::vector<std::shared_ptr<Shader>> shaders,
                    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments,
                    std::vector<vk::Format> colorAttachmentFormats, vk::Format depthStencilAttachmentFormat)
        : Pipeline(name, descriptorSetLayoutBindings, pushConstantRanges), mVertexBindings(vertexBindings),
          mVertexAttributes(vertexAttributes), mInputAssemblyState(inputAssemblyState),
          mRasterizationState(rasterizationState), mMultisampleState(multisampleState),
          mDepthStencilState(depthStencilState), mColorBlendState(colorBlendState), mShaders(shaders),
          mColorBlendAttachments(colorBlendAttachments), mColorAttachmentFormats(colorAttachmentFormats),
          mDepthStencilAttachmentFormat(depthStencilAttachmentFormat)
    {
        std::vector<ShaderResource *> shaderResources;
        for (const auto &shader : mShaders)
        {
            shaderResources.push_back(shader->GetResourceAs<ShaderResource>());
        }
        mResource = std::make_unique<GraphicPipelineResource>(
            mDescriptorSetLayoutBindings, mPushConstantRanges, mVertexBindings, mVertexAttributes, mInputAssemblyState,
            mRasterizationState, mMultisampleState, mDepthStencilState, mColorBlendState, shaderResources,
            mColorBlendAttachments, mColorAttachmentFormats, mDepthStencilAttachmentFormat);
    }
    ~GraphicPipeline() override = default;
};
} // namespace MEngine::Resource