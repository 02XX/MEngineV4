#include "ShaderResource.hpp"

namespace MEngine::Resource
{
void ShaderResource::InitRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.setCode(mCode);
    mShaderModule = device.createShaderModule(shaderModuleCreateInfo);
    if (!mShaderModule)
    {
        throw std::runtime_error("Failed to create shader module");
    }
};
void ShaderResource::ReleaseRHI(std::shared_ptr<Context> context)
{
    auto device = context->Device.get();
    if (mShaderModule)
    {
        device.destroyShaderModule(mShaderModule);
    }
}
} // namespace MEngine::Resource