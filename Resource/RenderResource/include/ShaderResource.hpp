#pragma once
#include "RHIHandler.hpp"
#include "RHIShader.hpp"
#include "RenderResource.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Shader;
class ShaderResource final : public RenderResource
{
  protected:
    RHIHandler<RHIShader> mRHIShaderHandler;
    Shader *mShader{nullptr};

  public:
    ShaderResource(Shader *shader) : RenderResource(), mShader(shader)
    {
    }
    ~ShaderResource() override = default;
    void InitRHI() override;
    void ReleaseRHI() override;
    inline RHIHandler<RHIShader> GetRHIShader() const
    {
        return mRHIShaderHandler;
    }
};
} // namespace MEngine::Resource
