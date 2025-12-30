#include "RHIDescriptorSet.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHIDescriptorSet::RHIDescriptorSet(const RHIDescriptorSetDesc &desc) : RHIResource(), mDescriptorSetDesc(desc)
{
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.setDescriptorPool(RHIContext::Instance().GetDescriptorPool())
        .setSetLayouts(mDescriptorSetDesc.SetLayouts)
        .setDescriptorSetCount(static_cast<uint32_t>(mDescriptorSetDesc.SetLayouts.size()));
    mDescriptorSets = RHIContext::Instance().GetDevice().allocateDescriptorSetsUnique(allocInfo);
    if (mDescriptorSets.empty())
    {
        throw std::runtime_error("Failed to allocate descriptor sets!");
    }
}
} // namespace MEngine::Platform