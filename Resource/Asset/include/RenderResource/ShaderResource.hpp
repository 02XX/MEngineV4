#pragma once
#include "RenderResource.hpp"

using namespace MEngine::Platform;
namespace MEngine::Resource
{
class Shader;
class ShaderResource final : public RenderResource
{
  private:
    vk::ShaderModule mShaderModule{nullptr};

  public:
    ShaderResource(Shader *shader);
    ~ShaderResource() override = default;
    void InitRHI(std::shared_ptr<Context> context) override;
    void ReleaseRHI(std::shared_ptr<Context> context) override;
    inline const vk::ShaderModule GetShaderModule() const
    {
        return mShaderModule;
    }
};
} // namespace MEngine::Resource
