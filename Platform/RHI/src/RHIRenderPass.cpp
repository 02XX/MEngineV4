#include "RHIRenderPass.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHIRenderPass::RHIRenderPass(const RHIRenderPassDesc &desc) : RHIResource(), mRenderPassDesc(desc)
{
    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.setAttachments(mRenderPassDesc.mAttachments)
        .setSubpasses(mRenderPassDesc.mSubpasses)
        .setDependencies(mRenderPassDesc.mDependencies);
    mRenderPass = RHIContext::Instance().GetDevice().createRenderPassUnique(renderPassCreateInfo);
    if (!mRenderPass)
    {
        throw std::runtime_error("Failed to create render pass");
    }
}
} // namespace MEngine::Platform