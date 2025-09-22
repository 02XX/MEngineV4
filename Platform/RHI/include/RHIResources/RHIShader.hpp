#pragma once
#include "RHIResource.hpp"
#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace MEngine::Platform
{
struct RHIShaderDesc
{
    std::vector<uint32_t> Code{};
    vk::ShaderStageFlagBits Stage{vk::ShaderStageFlagBits::eVertex};
};
class RHIShader final : public RHIResource
{
  private:
    vk::UniqueShaderModule mShaderModule{nullptr};
    RHIShaderDesc mShaderDesc{};

  public:
    RHIShader(const RHIShaderDesc &desc);
    ~RHIShader() override = default;
    inline vk::ShaderModule GetShaderModule() const
    {
        return mShaderModule.get();
    }
    inline vk::ShaderStageFlagBits GetShaderStage() const
    {
        return mShaderDesc.Stage;
    }
};
} // namespace MEngine::Platform