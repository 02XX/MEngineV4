#pragma once

#include "Asset.hpp"
#include "GraphicPipelineResource.hpp"
#include <string>
namespace MEngine::Resource
{
struct GraphicPipelineState
{
    vk::PipelineRasterizationStateCreateInfo RasterizationState{};
    vk::PipelineMultisampleStateCreateInfo MultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo DepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo ColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> ColorBlendAttachments{};
    std::string RenderPassName;
    uint32_t SubpassIndex = 0;
    std::vector<vk::DescriptorSetLayout> DescriptorSetLayouts{};
    std::vector<vk::PushConstantRange> PushConstantRanges{};
    vk::PipelineLayoutCreateFlags Flags{vk::PipelineLayoutCreateFlags()};
};
class GraphicPipeline : public Asset
{
    friend class GraphicPipelineResource;

  private:
    std::unique_ptr<GraphicPipelineResource> mResource;

  public:
    GraphicPipeline() : Asset()
    {
        mResource = std::make_unique<GraphicPipelineResource>(this);
    }
    ~GraphicPipeline() override = default;
    inline GraphicPipelineResource *GetResource() const
    {
        return mResource.get();
    }
};
} // namespace MEngine::Resource