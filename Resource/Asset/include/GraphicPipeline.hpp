#pragma once
#include "Asset.hpp"
#include "GraphicPipelineResource.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "ShaderResource.hpp"
#include "Vertex.hpp"
#include <memory>
#include <string>
#include <vulkan/vulkan_enums.hpp>

namespace MEngine::Resource
{
class GraphicPipeline final : public Pipeline
{
    friend class GraphicPipelineResource;

  private:
    std::vector<Shader *> mShaders{};
    std::vector<vk::Format> mColorAttachmentFormats{};
    vk::Format mDepthStencilAttachmentFormat{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};
    // Dynamic
  public:
    vk::VertexInputBindingDescription mVertexBindings{};
    vk::PipelineRasterizationStateCreateInfo mRasterizationState{};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{};

  public:
    GraphicPipeline(const std::string &name, std::vector<vk::DescriptorSetLayout> descriptorSetLayouts,
                    std::vector<vk::PushConstantRange> pushConstantRanges,
                    std::vector<vk::VertexInputAttributeDescription> vertexAttributes,
                    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState, std::vector<Shader *> shaders,
                    std::vector<vk::Format> colorAttachmentFormats, vk::Format depthStencilAttachmentFormat,
                    vk::PipelineMultisampleStateCreateInfo multisampleState,
                    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments)
        : Pipeline(name, descriptorSetLayouts, pushConstantRanges), mVertexAttributes(vertexAttributes),
          mInputAssemblyState(inputAssemblyState), mShaders(shaders), mColorAttachmentFormats(colorAttachmentFormats),
          mDepthStencilAttachmentFormat(depthStencilAttachmentFormat), mMultisampleState(multisampleState),
          mColorBlendAttachments(colorBlendAttachments)
    {

        std::vector<ShaderResource *> shaderResources;
        for (const auto &shader : mShaders)
        {
            shaderResources.push_back(shader->GetResourceAs<ShaderResource>());
        }
        mResource = std::make_unique<GraphicPipelineResource>(this);
    }
};
} // namespace MEngine::Resource