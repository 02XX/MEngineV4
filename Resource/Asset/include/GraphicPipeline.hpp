#pragma once

#include "Asset.hpp"
#include "GraphicPipelineResource.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include <memory>
#include <string>
namespace MEngine::Resource
{
struct GraphicPipelineState
{
    vk::PipelineInputAssemblyStateCreateInfo InputAssemblyState{};
    vk::PipelineRasterizationStateCreateInfo RasterizationState{};
    vk::PipelineMultisampleStateCreateInfo MultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo DepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo ColorBlendState{};
    std::vector<std::shared_ptr<Shader>> Shaders{};
    std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachments{};
    std::vector<vk::Format> ColorAttachmentFormats{};
    vk::Format DepthStencilAttachmentFormat{vk::Format::eD32Sfloat};
};

class GraphicPipeline : public Pipeline
{
    friend class GraphicPipelineResource;
    friend class GraphicPipelineBuilder;
    friend class GBufferPipelineBuilder;
    friend class GraphicPipelineManager;

  private:
    GraphicPipelineState mGraphicPipelineState{};

  protected:
    GraphicPipeline() : Pipeline()
    {
    }

  public:
    GraphicPipeline(const std::string &name, const GraphicPipelineState &state, const PipelineLayoutDesc &layoutDesc)
        : Pipeline(name, layoutDesc), mGraphicPipelineState(state)
    {
        mResource = std::make_unique<GraphicPipelineResource>(this);
    }
    ~GraphicPipeline() override = default;
};
} // namespace MEngine::Resource