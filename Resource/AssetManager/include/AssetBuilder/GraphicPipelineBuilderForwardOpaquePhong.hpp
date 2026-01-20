#pragma once
#pragma once
#include "GraphicPipelineBuilderPhong.hpp"
namespace MEngine::Resource
{
class GraphicPipelineBuilderForwardOpaquePhong final : public GraphicPipelineBuilderPhong,
                                                       public virtual IGraphicPipelineBuilder
{
  public:
    GraphicPipelineBuilderForwardOpaquePhong(std::shared_ptr<Context> context,
                                             std::shared_ptr<IManager<Shader>> shaderManager)
        : GraphicPipelineBuilderPhong(context, shaderManager)
    {
    }
    ~GraphicPipelineBuilderForwardOpaquePhong() override = default;
    virtual void SetShader() override;
    virtual void SetColorBlendState() override;
    virtual void SetColorAttachmentFormats() override;
    virtual void SetName() override;
};
} // namespace MEngine::Resource