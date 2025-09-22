#include "MaterialResource.hpp"
#include "GraphicPipeline.hpp"
#include "GraphicPipelineResource.hpp"
#include "Material.hpp"
namespace MEngine::Resource
{
void MaterialResource::InitRHI()
{
    RHIDescriptorSetDesc desc{};
    auto pipelineResource = mMaterial->mPipeline->GetResourceAs<GraphicPipelineResource>();
    desc.SetLayouts = pipelineResource->GetGraphicPipeline()->GetDescriptorSetLayouts();
    mDescriptorSetHandler = RHIHandler<RHIDescriptorSet>(new RHIDescriptorSet(desc));
}
void MaterialResource::ReleaseRHI()
{
    mDescriptorSetHandler.SafeRelease();
}
} // namespace MEngine::Resource