#pragma once
#pragma once
#include "GraphicPipelineBuilder.hpp"

namespace MEngine::Resource
{
class GraphicPipelineBuilderPBR : public GraphicPipelineBuilder, public virtual IGraphicPipelineBuilder
{
  public:
    GraphicPipelineBuilderPBR(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : GraphicPipelineBuilder(context, shaderManager)
    {
    }
    ~GraphicPipelineBuilderPBR() override = default;
    virtual void SetLayout() override;
    virtual void SetName() override;
};
} // namespace MEngine::Resource