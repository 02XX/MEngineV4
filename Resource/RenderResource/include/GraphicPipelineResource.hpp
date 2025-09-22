#pragma once

#include "RHIGraphicPipeline.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class GraphicPipeline;
class GraphicPipelineResource final : public RenderResource
{
  protected:
    RHIHandler<RHIGraphicPipeline> mGraphicPipelineHandler;
    GraphicPipeline *mGraphicPipeline{nullptr};

  public:
    GraphicPipelineResource(GraphicPipeline *graphicPipeline);
    virtual ~GraphicPipelineResource() = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    inline RHIHandler<RHIGraphicPipeline> GetGraphicPipeline() const
    {
        return mGraphicPipelineHandler;
    }
};
} // namespace MEngine::Resource