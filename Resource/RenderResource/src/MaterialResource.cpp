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
    desc.SetLayouts = {pipelineResource->GetGraphicPipeline()->GetDescriptorSetLayouts()[1]};
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mDescriptorSetHandlers[i] = RHIHandler<RHIDescriptorSet>(new RHIDescriptorSet(desc));
    }
}
void MaterialResource::ReleaseRHI()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        mDescriptorSetHandlers[i].SafeRelease();
    }
}
} // namespace MEngine::Resource