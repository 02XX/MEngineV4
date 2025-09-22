#include "ShaderResource.hpp"
#include "RHIHandler.hpp"
#include "RHIShader.hpp"
#include "Shader.hpp"
namespace MEngine::Resource
{
void ShaderResource::InitRHI()
{
    RHIShaderDesc desc;
    desc.Code = mShader->SPIRVCode;
    desc.Stage = mShader->mStage;
    mRHIShaderHandler = RHIHandler<RHIShader>(new RHIShader(desc));
};
void ShaderResource::ReleaseRHI()
{
    mRHIShaderHandler.SafeRelease();
}
} // namespace MEngine::Resource