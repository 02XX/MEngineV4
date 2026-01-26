#include "PipelineManager.hpp"
#include "Context.hpp"
#include "Logger.hpp"
#include "Math.hpp"
#include "Pipeline.hpp"
#include "PipelineResource.hpp"
#include "Shader.hpp"
#include "ShaderManager.hpp"
#include "Vertex.hpp"
#include <memory>
#include <string>
#include <vector>

namespace MEngine::Resource
{
PipelineManager::PipelineManager(std::shared_ptr<Context> context, std::shared_ptr<ShaderManager> shaderManager)
    : Manager<Pipeline, PipelineResource, PipelineEntity>(context), mShaderManager(shaderManager)
{
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.setPoolSizes(sDescriptorPoolSize)
        .setMaxSets(MAX_DESCRIPTOR_COUNT * sDescriptorPoolSize.size())
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet |
                  vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind);
    mDescriptorPool = context->Device->createDescriptorPool(descriptorPoolCreateInfo);
    if (!mDescriptorPool)
    {
        throw std::runtime_error("Failed to create descriptor pool");
    }
    for (auto &[layoutName, bindings] : sDefaultDescriptorSetLayoutBindings)
    {
        std::vector<vk::DescriptorBindingFlags> bindingFlags(bindings.size(),
                                                             vk::DescriptorBindingFlagBits::eUpdateAfterBind |
                                                                 vk::DescriptorBindingFlagBits::ePartiallyBound);

        vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{};
        bindingFlagsCreateInfo.setBindingFlags(bindingFlags);

        vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{};
        vk::DescriptorSetLayoutCreateFlags flags = vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool;

        layoutCreateInfo.setBindings(bindings).setFlags(flags).setPNext(&bindingFlagsCreateInfo);

        auto descriptorSetLayout = context->Device->createDescriptorSetLayout(layoutCreateInfo);
        if (!descriptorSetLayout)
        {
            throw std::runtime_error("Failed to create descriptor set layout");
        }

        mDefaultDescriptorSetLayouts.insert({layoutName, descriptorSetLayout});
    }

    // Create Default Pipelines
    std::vector<std::string> genericDescriptorSetLayouts{
        DefaultDescriptorSetLayoutType::Global,
        DefaultDescriptorSetLayoutType::Bindless,
        DefaultDescriptorSetLayoutType::Material,
    };
    std::vector<std::string> genericPushConstantRanges{
        DefaultPushConstantRangeType::Matrix,
    };
    std::vector<vk::PipelineColorBlendAttachmentState> mColorBlendAttachments{
        vk::PipelineColorBlendAttachmentState{}
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA),
        vk::PipelineColorBlendAttachmentState{}
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA),
        vk::PipelineColorBlendAttachmentState{}
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA),
        vk::PipelineColorBlendAttachmentState{}
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA),
        vk::PipelineColorBlendAttachmentState{}
            .setBlendEnable(vk::False)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eZero)
            .setColorBlendOp(vk::BlendOp::eAdd)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
            .setAlphaBlendOp(vk::BlendOp::eAdd)
            .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA),
    };
    vk::PipelineMultisampleStateCreateInfo mMultisampleState{};
    mMultisampleState.setRasterizationSamples(vk::SampleCountFlagBits::e1)
        .setSampleShadingEnable(vk::False)
        .setMinSampleShading(1.0f)
        .setPSampleMask(nullptr)
        .setAlphaToCoverageEnable(vk::False)
        .setAlphaToOneEnable(vk::False);
    std::vector<vk::VertexInputAttributeDescription> mVertexAttributes{};
    mVertexAttributes = Vertex::GetVertexInputAttributeDescription();
    vk::VertexInputBindingDescription vertexBindings = Vertex::GetVertexInputBindingDescription();
    vk::PipelineInputAssemblyStateCreateInfo mInputAssemblyState{};
    mInputAssemblyState.setPrimitiveRestartEnable(vk::False).setTopology(vk::PrimitiveTopology::eTriangleList);
    std::vector<std::shared_ptr<Shader>> shaders{
        mShaderManager->GetByNameAs<Shader>(DefaultShaderType::ForwardOpaquePhongVert),
        mShaderManager->GetByNameAs<Shader>(DefaultShaderType::ForwardOpaquePhongFrag)};
    // Graphic Pipelines
    auto graphicForwardOpaquePhong = std::make_shared<GraphicPipeline>(
        DefaultGraphicPipelineType::ForwardOpaquePhong, genericDescriptorSetLayouts, genericPushConstantRanges,
        vertexBindings, mVertexAttributes, mInputAssemblyState, shaders, sMRTFormats, sDepthStencilFormat,
        mColorBlendAttachments, mMultisampleState);
    graphicForwardOpaquePhong->SetID(sDefaultPipelines.at(DefaultGraphicPipelineType::ForwardOpaquePhong));
    Add(graphicForwardOpaquePhong);
}
PipelineManager::~PipelineManager()
{
    for (const auto &[name, layout] : mDefaultDescriptorSetLayouts)
    {
        mContext->Device->destroyDescriptorSetLayout(layout);
    }
    mDefaultDescriptorSetLayouts.clear();
    if (mDescriptorPool)
    {
        mContext->Device->destroyDescriptorPool(mDescriptorPool);
        mDescriptorPool = nullptr;
    }
}
std::shared_ptr<GraphicPipeline> PipelineManager::GetGraphicPipeline(const Core::UUID &id)
{
    return std::dynamic_pointer_cast<GraphicPipeline>(Get(id));
}
std::shared_ptr<GraphicPipeline> PipelineManager::GetGraphicPipelineByName(const std::string &name)
{
    return std::dynamic_pointer_cast<GraphicPipeline>(GetByName(name));
}
} // namespace MEngine::Resource