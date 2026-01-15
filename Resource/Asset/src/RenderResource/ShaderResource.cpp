#include "ShaderResource.hpp"
#include "Shader.hpp"
namespace MEngine::Resource
{
ShaderResource::ShaderResource(Shader *shader) : RenderResource(shader)
{
}
void ShaderResource::InitRHI(std::shared_ptr<Context> context)
{
    auto shader = static_cast<Shader *>(mOwnerAsset);
    auto device = context->Device.get();
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{};
    shaderModuleCreateInfo.setCode(shader->mSPIRVCode);
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