#include "RHIShader.hpp"
#include "RHIContext.hpp"
namespace MEngine::Platform
{
RHIShader::RHIShader(const RHIShaderDesc &desc) : mShaderDesc(desc)
{
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.setCode(mShaderDesc.Code);
    mShaderModule = RHIContext::Instance().GetDevice().createShaderModuleUnique(shaderModuleCreateInfo);
    if (!mShaderModule)
    {
        throw std::runtime_error("Failed to create shader module");
    }
}
} // namespace MEngine::Platform