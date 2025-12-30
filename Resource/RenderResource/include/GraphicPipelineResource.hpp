#pragma once

#include "RHIContext.hpp"
#include "RHIDescriptorSet.hpp"
#include "RHIGraphicPipeline.hpp"
#include "RHIHandler.hpp"
#include "RenderResource.hpp"
#include <array>
using namespace MEngine::Platform;
namespace MEngine::Resource
{
class GraphicPipeline;
class GraphicPipelineResource final : public RenderResource
{
  protected:
    RHIHandler<RHIGraphicPipeline> mGraphicPipelineHandler;
    GraphicPipeline *mGraphicPipeline{nullptr};
    std::array<RHIHandler<RHIDescriptorSet>, MAX_FRAMES_IN_FLIGHT> mPerFrameDescriptorSetHandlers{};

  public:
    GraphicPipelineResource(GraphicPipeline *graphicPipeline);
    virtual ~GraphicPipelineResource() = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    inline RHIHandler<RHIGraphicPipeline> GetGraphicPipeline() const
    {
        return mGraphicPipelineHandler;
    }
    inline RHIHandler<RHIDescriptorSet> GetPerFrameDescriptorSet(int frameIndex) const
    {
        return mPerFrameDescriptorSetHandlers[frameIndex];
    }
};
} // namespace MEngine::Resource