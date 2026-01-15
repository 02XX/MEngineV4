#pragma once
#include "Builder.hpp"
#include "Context.hpp"
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
    std::shared_ptr<Context> mContext;
    std::shared_ptr<IManager<Shader>> mShaderManager;

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> mDescriptorSetLayoutBindings{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

    std::vector<std::shared_ptr<Shader>> mShaders{};
    vk::VertexInputBindingDescription mVertexBindings{};
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};
    vk::PipelineRasterizationStateCreateInfo mRasterizationState{};
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    vk::PipelineDepthStencilStateCreateInfo mDepthStencilState{};
    vk::PipelineColorBlendStateCreateInfo mColorBlendState{};
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{};
    std::vector<vk::Format> mColorAttachmentFormats{};
    vk::Format mDepthStencilAttachmentFormat{};

  public:
    GraphicPipelineBuilder(std::shared_ptr<Context> context, std::shared_ptr<IManager<Shader>> shaderManager)
        : Builder<GraphicPipeline>(), mContext(context), mShaderManager(shaderManager)
    {
    }
    ~GraphicPipelineBuilder() override = default;
    virtual void Reset() override;
    virtual std::unique_ptr<GraphicPipeline> Build() override;
    virtual void SetVertexInputState() override;
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