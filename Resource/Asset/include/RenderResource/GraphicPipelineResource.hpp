#pragma once

#include "Context.hpp"
#include "PipelineResource.hpp"
#include "ShaderResource.hpp"
namespace MEngine::Resource
{
class GraphicPipeline;
class GraphicPipelineResource final : public PipelineResource
{

  public:
    GraphicPipelineResource(GraphicPipeline *pipeline);
    ~GraphicPipelineResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
};
} // namespace MEngine::Resource