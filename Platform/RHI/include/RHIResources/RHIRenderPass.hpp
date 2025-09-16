#pragma once
#include "RHIResource.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Platform
{
struct RHIRenderPassDesc
{
    std::vector<vk::AttachmentDescription> mAttachments{};
    std::vector<vk::SubpassDescription> mSubpasses{};
    std::vector<vk::SubpassDependency> mDependencies{};
};
class RHIRenderPass : public RHIResource
{
  protected:
    vk::UniqueRenderPass mRenderPass{nullptr};
    RHIRenderPassDesc mRenderPassDesc{};

  public:
    RHIRenderPass(const RHIRenderPassDesc &desc);
    ~RHIRenderPass() override = default;
};
} // namespace MEngine::Platform