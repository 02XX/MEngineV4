#pragma once
#include "Asset.hpp"
#include "GraphicPipeline.hpp"
#include "MaterialResource.hpp"
#include <memory>
#include <string>

namespace MEngine::Resource
{

// Abstract class
using DepthSetting = vk::PipelineDepthStencilStateCreateInfo;
using RasterizationSetting = vk::PipelineRasterizationStateCreateInfo;
using InputAssemblySetting = vk::PipelineInputAssemblyStateCreateInfo;
class Material : public Asset
{
    friend class MaterialResource;

  public:
    std::shared_ptr<GraphicPipeline> mPipeline{};
    bool mDynamic{false};
    DepthSetting mDepthSetting{DepthSetting()
                                   .setDepthTestEnable(vk::True)
                                   .setDepthWriteEnable(vk::True)
                                   .setDepthCompareOp(vk::CompareOp::eLess)
                                   .setDepthBoundsTestEnable(vk::False)
                                   .setMinDepthBounds(0.0f)
                                   .setMaxDepthBounds(1.0f)
                                   .setStencilTestEnable(vk::False)
                                   .setBack({})
                                   .setFront({})

    };
    RasterizationSetting mRasterizationSetting{RasterizationSetting()
                                                   .setCullMode(vk::CullModeFlagBits::eBack)
                                                   .setFrontFace(vk::FrontFace::eCounterClockwise)
                                                   .setPolygonMode(vk::PolygonMode::eFill)
                                                   .setLineWidth(1.0f)
                                                   .setDepthClampEnable(vk::False)
                                                   .setDepthBiasEnable(vk::False)
                                                   .setDepthBiasClamp(0.0f)
                                                   .setDepthBiasConstantFactor(0.0f)
                                                   .setDepthBiasSlopeFactor(0.0f)
                                                   .setRasterizerDiscardEnable(vk::False)

    };

    InputAssemblySetting mInputAssemblySetting{
        InputAssemblySetting().setTopology(vk::PrimitiveTopology::eTriangleList).setPrimitiveRestartEnable(vk::False)};

  protected:
    Material() : Asset()
    {
        mAssetType = AssetType::Material;
    }

  public:
    Material(const std::string &name, std::shared_ptr<GraphicPipeline> pipeline, bool dynamic = false)
        : Asset(name), mPipeline(pipeline), mDynamic(dynamic)
    {
        mResource = std::make_unique<MaterialResource>(this);
    }
};
} // namespace MEngine::Resource