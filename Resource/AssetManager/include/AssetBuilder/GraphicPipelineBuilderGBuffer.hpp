#pragma once
#include "GraphicPipelineBuilder.hpp"

namespace MEngine::Resource
{
class GraphicPipelineBuilderGBuffer : public GraphicPipelineBuilder, public virtual IGraphicPipelineBuilder
{
  public:
    GraphicPipelineBuilderGBuffer(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : GraphicPipelineBuilder(context, shaderManager)
    {
    }
    ~GraphicPipelineBuilderGBuffer() override = default;
    virtual void SetShader() override;
    virtual void SetColorBlendState() override;
    virtual void SetColorAttachmentFormats() override;
    virtual void SetLayout() override;
    virtual void SetName() override;
};
} // namespace MEngine::Resource