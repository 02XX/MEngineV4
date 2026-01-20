#pragma once
#pragma once
#include "GraphicPipelineBuilder.hpp"

namespace MEngine::Resource
{
class GraphicPipelineBuilderPhong : public GraphicPipelineBuilder, public virtual IGraphicPipelineBuilder
{
  public:
    GraphicPipelineBuilderPhong(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : GraphicPipelineBuilder(context, shaderManager)
    {
    }
    ~GraphicPipelineBuilderPhong() override = default;
    virtual void SetLayout() override;
    virtual void SetName() override;
};
} // namespace MEngine::Resource