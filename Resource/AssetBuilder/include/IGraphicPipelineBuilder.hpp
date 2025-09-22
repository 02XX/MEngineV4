#pragma once
#include "GraphicPipeline.hpp"
#include "IBuilder.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Resource
{
class IGraphicPipelineBuilder : public virtual IBuilder<GraphicPipeline>
{
  public:
    ~IGraphicPipelineBuilder() override = default;
    virtual void SetInputAssemblyState() = 0;
    virtual void SetShader() = 0;
    virtual void SetRasterizationState() = 0;
    virtual void SetMultiSampleState() = 0;
    virtual void SetDepthStencilState() = 0;
    virtual void SetColorBlendState() = 0;
    virtual void SetColorAttachmentFormats() = 0;
    virtual void SetDepthStencilAttachmentFormat() = 0;
    virtual void SetLayout() = 0;
};
} // namespace MEngine::Resource