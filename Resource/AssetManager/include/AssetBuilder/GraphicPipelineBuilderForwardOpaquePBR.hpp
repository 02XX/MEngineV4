#pragma once
#pragma once
#include "GraphicPipelineBuilderPBR.hpp"
namespace MEngine::Resource
{
class GraphicPipelineBuilderForwardOpaquePBR final : public GraphicPipelineBuilderPBR,
                                                     public virtual IGraphicPipelineBuilder
{
  public:
    GraphicPipelineBuilderForwardOpaquePBR(std::shared_ptr<Context> context,
                                           std::shared_ptr<IManager<Shader>> shaderManager)
        : GraphicPipelineBuilderPBR(context, shaderManager)
    {
    }
    ~GraphicPipelineBuilderForwardOpaquePBR() override = default;
    virtual void SetShader() override;
    virtual void SetColorBlendState() override;
    virtual void SetColorAttachmentFormats() override;
    virtual void SetName() override;
};
} // namespace MEngine::Resource