#include "GraphicPipelineResource.hpp"

namespace MEngine::Resource
{
GraphicPipelineResource::GraphicPipelineResource(GraphicPipeline *graphicPipeline)
    : RenderResource(), mGraphicPipeline(graphicPipeline)
{
}
void GraphicPipelineResource::InitRHI() {
    
};
void GraphicPipelineResource::ReleaseRHI()
{
    mGraphicPipelineHandler.SafeRelease();
}
} // namespace MEngine::Resource