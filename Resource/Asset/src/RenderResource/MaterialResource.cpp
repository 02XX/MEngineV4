#include "MaterialResource.hpp"
#include "AssetManager.hpp"
#include "Material.hpp"
namespace MEngine::Resource
{
MaterialResource::MaterialResource(Material *material) : RenderResource(material)
{
}
void MaterialResource::InitRHI(std::shared_ptr<Context> context)
{
}
void MaterialResource::ReleaseRHI(std::shared_ptr<Context> context)
{
}
void MaterialResource::Bind(BindContext bindContext)
{
    auto material = static_cast<Material *>(mOwnerAsset);
    // 输入装配
    bindContext.CommandBuffer.setPrimitiveTopology(material->mInputAssemblySetting.topology);
    bindContext.CommandBuffer.setPrimitiveRestartEnable(material->mInputAssemblySetting.primitiveRestartEnable);
    // 深度
    bindContext.CommandBuffer.setDepthTestEnable(material->mDepthSetting.depthTestEnable);
    bindContext.CommandBuffer.setDepthWriteEnable(material->mDepthSetting.depthWriteEnable);
    bindContext.CommandBuffer.setDepthCompareOp(material->mDepthSetting.depthCompareOp);
    bindContext.CommandBuffer.setDepthBoundsTestEnable(material->mDepthSetting.depthBoundsTestEnable);
    bindContext.CommandBuffer.setDepthBounds(material->mDepthSetting.minDepthBounds,
                                             material->mDepthSetting.maxDepthBounds);
    bindContext.CommandBuffer.setStencilTestEnable(material->mDepthSetting.stencilTestEnable);
    bindContext.CommandBuffer.setStencilOp(
        vk::StencilFaceFlagBits::eFront, material->mDepthSetting.front.failOp, material->mDepthSetting.front.passOp,
        material->mDepthSetting.front.depthFailOp, material->mDepthSetting.front.compareOp);

    // 光栅化
    bindContext.CommandBuffer.setCullMode(material->mRasterizationSetting.cullMode);
    bindContext.CommandBuffer.setFrontFace(material->mRasterizationSetting.frontFace);
    bindContext.CommandBuffer.setLineWidth(material->mRasterizationSetting.lineWidth);
    bindContext.CommandBuffer.setDepthBiasEnable(material->mRasterizationSetting.depthBiasEnable);
    bindContext.CommandBuffer.setDepthBias(material->mRasterizationSetting.depthBiasConstantFactor,
                                           material->mRasterizationSetting.depthBiasClamp,
                                           material->mRasterizationSetting.depthBiasSlopeFactor);
}
} // namespace MEngine::Resource