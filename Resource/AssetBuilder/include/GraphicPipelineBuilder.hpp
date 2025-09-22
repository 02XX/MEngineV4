#pragma once
#include "Builder.hpp"
#include "GraphicPipeline.hpp"
#include "IGraphicPipelineBuilder.hpp"
#include "IManager.hpp"
#include "Pipeline.hpp"
#include <memory>

namespace MEngine::Resource
{
class GraphicPipelineBuilder : public Builder<GraphicPipeline>, public virtual IGraphicPipelineBuilder
{

  protected:
    std::shared_ptr<IManager<Shader>> mShaderManager;

    std::unique_ptr<GraphicPipeline> mGraphicPipeline;
    GraphicPipelineState mGraphicPipelineState{};
    PipelineLayoutDesc mPipelineLayoutDesc{};

  public:
    GraphicPipelineBuilder(std::shared_ptr<IManager<Shader>> shaderManager)
        : Builder<GraphicPipeline>(), mShaderManager(shaderManager)
    {
    }
    ~GraphicPipelineBuilder() override = default;
    virtual void Reset() override;
    virtual std::unique_ptr<GraphicPipeline> Build() override;
    virtual void SetInputAssemblyState() override;
    virtual void SetRasterizationState() override;
    virtual void SetMultiSampleState() override;
    virtual void SetDepthStencilState() override;
    virtual void SetColorBlendState() override;
    virtual void SetColorAttachmentFormats() override;
    virtual void SetDepthStencilAttachmentFormat() override;
    virtual void SetLayout() override;
};
} // namespace MEngine::Resource