#pragma once
#include "GraphicPipelineBuilder.hpp"

namespace MEngine::Resource
{
class GBufferPipelineBuilder : public GraphicPipelineBuilder, public virtual IGraphicPipelineBuilder
{
  public:
    GBufferPipelineBuilder(std::shared_ptr<IManager<Shader>> shaderManager) : GraphicPipelineBuilder(shaderManager)
    {
    }
    ~GBufferPipelineBuilder() override = default;
    virtual void SetShader() override;
    virtual void SetColorBlendState() override;
    virtual void SetColorAttachmentFormats() override;
    virtual void SetLayout() override;
    virtual void SetName() override;
};
} // namespace MEngine::Resource