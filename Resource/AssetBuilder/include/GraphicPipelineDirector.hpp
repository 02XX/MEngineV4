#pragma once
#include "IGraphicPipelineBuilder.hpp"

namespace MEngine::Resource
{

class GraphicPipelineDirector
{
  public:
    std::unique_ptr<GraphicPipeline> Make(IGraphicPipelineBuilder &builder);
};
} // namespace MEngine::Resource