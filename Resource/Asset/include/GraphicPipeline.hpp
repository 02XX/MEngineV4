#pragma once
#include "Asset.hpp"
#include "GraphicPipelineResource.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "ShaderResource.hpp"
#include "Vertex.hpp"
#include <memory>
#include <string>

namespace MEngine::Resource
{
class GraphicPipeline final : public Pipeline
{
    friend class GraphicPipelineResource;

  public:
    std::vector<std::shared_ptr<Shader>> mShaders{};
    std::vector<vk::Format> mColorAttachmentFormats{};
    vk::Format mDepthStencilAttachmentFormat{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    // 顶点输入
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::VertexInputBindingDescription mVertexInputBinding{vk::VertexInputBindingDescription()
                                                              .setBinding(0)
                                                              .setStride(sizeof(Vertex))
                                                              .setInputRate(vk::VertexInputRate::eVertex)};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{vk::PipelineInputAssemblyStateCreateInfo()
                                                                     .setTopology(vk::PrimitiveTopology::eTriangleList)
                                                                     .setPrimitiveRestartEnable(vk::False)};
    // 多重采样
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{vk::PipelineMultisampleStateCreateInfo()
                                                                 .setRasterizationSamples(vk::SampleCountFlagBits::e1)
                                                                 .setSampleShadingEnable(vk::False)};

  public:
    // Dynamic
    vk::PipelineRasterizationStateCreateInfo mRasterizationState{vk::PipelineRasterizationStateCreateInfo()
                                                                     .setCullMode(vk::CullModeFlagBits::eBack)
                                                                     .setFrontFace(vk::FrontFace::eCounterClockwise)
                                                                     .setPolygonMode(vk::PolygonMode::eFill)
                                                                     .setLineWidth(1.0f)
                                                                     .setDepthClampEnable(vk::False)
                                                                     .setDepthBiasEnable(vk::False)
                                                                     .setDepthBiasClamp(0.0f)
                                                                     .setDepthBiasConstantFactor(0.0f)
                                                                     .setDepthBiasSlopeFactor(0.0f)
                                                                     .setRasterizerDiscardEnable(vk::False)};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{vk::PipelineDepthStencilStateCreateInfo()
                                                                   .setDepthTestEnable(vk::True)
                                                                   .setDepthWriteEnable(vk::True)
                                                                   .setDepthCompareOp(vk::CompareOp::eLess)
                                                                   .setDepthBoundsTestEnable(vk::False)
                                                                   .setMinDepthBounds(0.0f)
                                                                   .setMaxDepthBounds(1.0f)
                                                                   .setStencilTestEnable(vk::False)
                                                                   .setBack({})
                                                                   .setFront({})};

  public:
    GraphicPipeline(const std::string &name, std::vector<std::string> descriptorSetLayouts,
                    std::vector<std::string> pushConstantRanges, vk::VertexInputBindingDescription vertexBindings,
                    std::vector<vk::VertexInputAttributeDescription> vertexAttributes,
                    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState,
                    std::vector<std::shared_ptr<Shader>> shaders, std::vector<vk::Format> colorAttachmentFormats,
                    vk::Format depthStencilAttachmentFormat,
                    std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachments,
                    vk::PipelineMultisampleStateCreateInfo multisampleState)
        : Pipeline(name, descriptorSetLayouts, pushConstantRanges), mVertexInputBinding(vertexBindings),
          mVertexAttributes(vertexAttributes), mInputAssemblyState(inputAssemblyState), mShaders(shaders),
          mColorAttachmentFormats(colorAttachmentFormats), mDepthStencilAttachmentFormat(depthStencilAttachmentFormat),
          mMultisampleState(multisampleState), mColorBlendAttachments(colorBlendAttachments)
    {
        mResource = std::make_unique<GraphicPipelineResource>(this);
    }
};
} // namespace MEngine::Resource