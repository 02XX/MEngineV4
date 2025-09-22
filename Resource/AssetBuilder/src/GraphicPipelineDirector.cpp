#include "GraphicPipelineDirector.hpp"
#include "Pipeline.hpp"
#include <memory>

namespace MEngine::Resource
{
std::unique_ptr<GraphicPipeline> GraphicPipelineDirector::Make(IGraphicPipelineBuilder &builder)
{
    builder.SetName();
    builder.SetInputAssemblyState();
    builder.SetShader();
    builder.SetRasterizationState();
    builder.SetMultiSampleState();
    builder.SetDepthStencilState();
    builder.SetColorBlendState();
    builder.SetColorAttachmentFormats();
    builder.SetDepthStencilAttachmentFormat();
    builder.SetLayout();
    return builder.Build();
}
} // namespace MEngine::Resource